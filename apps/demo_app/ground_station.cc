#include "ground_station.h"
#include "command_protocol.h"

#include <stream/util/udpip_connection.h>
#include <stream/func_filter.h>
#include <gs/3d_stream_view.h>
#include <gs/size_stream_view.h>
#include <gs/map_stream_view.h>
#include <gs/size_push_gen.h>
#include <gs/orientation_stream_view.h>
#include <gs/label_stream_view.h>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <qt4/Qt/qboxlayout.h>
#include <qt4/Qt/qgridlayout.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>

int stam;

GroundStation::KeyboardGetterWidget::KeyboardGetterWidget(
				 gs::SizePushGen* pitch,
				 gs::SizePushGen* roll):
		m_pitch(pitch),
		m_roll(roll)
{
	m_last_pitch = m_pitch->get_value();
	m_last_roll = m_roll->get_value();
}


void GroundStation::KeyboardGetterWidget::keyPressEvent(QKeyEvent *k) {
	switch (k->key()) {
		case Qt::Key_Down:
			m_last_pitch = m_pitch->get_value();
			m_pitch->set_value(-15.);
			break;
		case Qt::Key_Up:
			m_last_pitch = m_pitch->get_value();
			m_pitch->set_value(15.);
			break;
		case Qt::Key_Right:
			m_last_roll = m_roll->get_value();
			m_roll->set_value(40.);
			break;
		case Qt::Key_Left:
			m_last_roll = m_roll->get_value();
			m_roll->set_value(-40.);
			break;
	}
}

void GroundStation::KeyboardGetterWidget::keyReleaseEvent(QKeyEvent *k) {
	switch (k->key()) {
		case Qt::Key_Down: 		m_pitch->set_value(m_last_pitch); 	break;
		case Qt::Key_Up: 		m_pitch->set_value(m_last_pitch); 	break;
		case Qt::Key_Left: 		m_roll->set_value(m_last_roll);		break;
		case Qt::Key_Right: 	m_roll->set_value(m_last_roll);		break;
	}
}

GroundStation::GroundStation(std::string plane_address):
			m_wanted_alt(boost::make_shared<stream::PushToPopConv<float> >(0.)),
			m_control_connection(plane_address, commands::port),
			m_app(stam, (char**)0)
{
	std::cout << "Presenting UDP from addr " << plane_address << std::endl;

	//
	// Import all the streams
	//
	typedef boost::shared_ptr<stream::AsyncStreamConnection::SendStream> send_stream_ptr;
	typedef boost::shared_ptr<stream::AsyncStreamConnection::RecvStream> recv_stream_ptr;
	typedef stream::AsyncStreamConnection::RecvPopStream<lin_algebra::vec3f> vec3_recv_stream;
	typedef stream::AsyncStreamConnection::RecvPopStream<lin_algebra::vec2f> vec2_recv_stream;
	typedef stream::AsyncStreamConnection::RecvPopStream<float> float_recv_stream;
	typedef stream::AsyncStreamConnection::SendPushStream<float> float_send_stream;

	// create the streams
	boost::shared_ptr<vec3_recv_stream> watch_compass_sensor = 		boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<vec3_recv_stream> watch_acc_sensor = 			boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<vec3_recv_stream> gyro_watch_orientation =	boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<vec3_recv_stream> watch_rest_orientation =	boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<vec3_recv_stream> orientation = 				boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<float_recv_stream> airspeed = 				boost::make_shared<float_recv_stream>();
	boost::shared_ptr<float_recv_stream> reliability = 				boost::make_shared<float_recv_stream>();
	boost::shared_ptr<float_recv_stream> gyro_fps = 				boost::make_shared<float_recv_stream>();
	boost::shared_ptr<vec2_recv_stream> position = 					boost::make_shared<vec2_recv_stream>();
	boost::shared_ptr<float_recv_stream> alt = 						boost::make_shared<float_recv_stream>();
	boost::shared_ptr<float_recv_stream> battery = 					boost::make_shared<float_recv_stream>();
	boost::shared_ptr<float_recv_stream> gas_servo =				boost::make_shared<float_recv_stream>();
	boost::shared_ptr<float_send_stream> sa_roll_control = 			boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> sa_tilt_control = 			boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> sa_gas_control = 				boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> sa_yaw_control = 				boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> gas_control = 				boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> yaw_control = 				boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> roll_control = 			boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> pitch_control = 			boost::make_shared<float_send_stream>();

	stream::AsyncStreamConnection::send_streams_t send_streams = boost::assign::list_of
		((send_stream_ptr)sa_roll_control		)
		((send_stream_ptr)sa_tilt_control		)
		((send_stream_ptr)sa_gas_control		)
		((send_stream_ptr)sa_yaw_control		)
		((send_stream_ptr)gas_control			)
		((send_stream_ptr)yaw_control			)
		((send_stream_ptr)roll_control			)
		((send_stream_ptr)pitch_control			);



	// fill them into the recv_stream list
	stream::AsyncStreamConnection::recv_streams_t recv_streams = boost::assign::list_of
			((recv_stream_ptr)watch_compass_sensor   )
			((recv_stream_ptr)watch_acc_sensor       )
			((recv_stream_ptr)gyro_watch_orientation )
			((recv_stream_ptr)watch_rest_orientation )
			((recv_stream_ptr)orientation            )
			((recv_stream_ptr)airspeed               )
	        ((recv_stream_ptr)reliability            )
	        ((recv_stream_ptr)gyro_fps               )
            ((recv_stream_ptr)position             	 )
			((recv_stream_ptr)alt               	 )
			((recv_stream_ptr)battery              	 )
			((recv_stream_ptr)gas_servo            	 );

	// creating the udp connection stuff
	boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
			boost::make_shared<stream::UdpipConnectionFactory>(4444, plane_address, 5555);

	// create the async stream connection and run it
	m_connection = boost::make_shared<stream::AsyncStreamConnection>(
			send_streams,
			recv_streams,
			conn_factory,
			true,
			50.);
	m_connection->start();


	//
	// Create the ground station window
	//

	QMainWindow* wnd = new QMainWindow;
	const QSize frame_size = QApplication::desktop()->size()*0.80;
	wnd->setWindowState(Qt::WindowMaximized);

	// global parameters
	const QSize stream3d_dimention(frame_size.width()/2.05, frame_size.height()/2);
	const float view_update_time = 0.01;

	// the 3d orientation view
	gs::StreamView3d *view3d = new gs::StreamView3d(view_update_time, stream3d_dimention);
	view3d->addAngleStream(orientation, irr::core::vector3df(0., 0., 8.));
	view3d->addVecStream(watch_acc_sensor, irr::core::vector3df(0., 0., 8.));
	view3d->addVecStream(watch_compass_sensor, irr::core::vector3df(0., 0., 8.));

	// the 2d orientation view
	gs::OrientationStreamView* view2d = new gs::OrientationStreamView(orientation,
			view_update_time, stream3d_dimention.width());

	// the label orientation view
	gs::LabelStreamView<float>* pitch_view = new gs::LabelStreamView<float>(
			stream::create_func_pop_filter<lin_algebra::vec3f, float>(orientation, lin_algebra::get<0>),
			view_update_time,
			"Pitch");
	gs::LabelStreamView<float>* roll_view = new gs::LabelStreamView<float>(
			stream::create_func_pop_filter<lin_algebra::vec3f, float>(orientation, lin_algebra::get<2>),
			view_update_time,
			"Roll");
	gs::LabelStreamView<float>* yaw_view = new gs::LabelStreamView<float>(
			stream::create_func_pop_filter<lin_algebra::vec3f, float>(orientation, lin_algebra::get<1>),
			view_update_time,
			"Yaw");


	// the airspeed stream
	gs::SizeStreamView *view_airspeed = new gs::SizeStreamView(
			stream::create_func_pop_filter<float, float>(airspeed, boost::bind(std::multiplies<float>(), _1, 3.6f)),
			"Airspeed [km/h]", view_update_time, 0., 70.);

	// the reliable stream
	gs::SizeStreamView *view_reliability = new gs::SizeStreamView(reliability, "Reliability", view_update_time, 0., 1.);

	// the gyro_fps stream
	gs::SizeStreamView *view_fps = new gs::SizeStreamView(gyro_fps, "FPS", 0.1, 0., 400.);

	// the position
	const QSize map_dimention(frame_size.width()/2.3, frame_size.height());
	gs::MapStreamView* map_view = new gs::MapStreamView(position, 1.0f, map_dimention,
					std::string("../../projs/ground_station/data/map"));

	// the link quality
	gs::SizeStreamView* view_link_quality = new gs::SizeStreamView(
			m_connection->get_quality_stream(), "Link", 0.1f , 0., 1.);

	// the plane's alt
	gs::SizeStreamView* view_alt = new gs::SizeStreamView(alt, "Alt", 0.03f , 0., 200.);

	// the plane's bat
	gs::LabelStreamView<float>* view_bat = new gs::LabelStreamView<float>(battery, 1., "Bat");

	// the plane's gas servo
	gs::SizeStreamView *view_gas = new gs::SizeStreamView(gas_servo, "Gas", 0.03, 0., 100.);

	// the new waypoint's alt
	gs::SizePushGen* gen_waypoints_alt = new gs::SizePushGen(m_wanted_alt, "WP Alt", 0, 200., 100.);

	// the SA Pilot controllers
	m_sa_tilt_control_widget  = new gs::SizePushGen(sa_tilt_control, "SA Tilt", -15., 15., 0.);
	m_sa_roll_control_widget  = new gs::SizePushGen(sa_roll_control, "SA Roll", -40, 40., 0., gs::SizePushGen::HORIZONTAL_DIAGRAM);
	m_sa_gas_control_widget   = new gs::SizePushGen(sa_gas_control,  "SA Gas", 0, 100., 50.);
	m_sa_yaw_control_widget   = new gs::SizePushGen(sa_yaw_control,  "SA Yaw", 0, 100., 50., gs::SizePushGen::HORIZONTAL_DIAGRAM);

	// the No Pilot controllers
	m_np_tilt_control_widget  = new gs::SizePushGen(pitch_control, "Pitch", 	0., 100., 50.);
	m_np_roll_control_widget  = new gs::SizePushGen(roll_control,  "Roll", 	0., 100., 50., gs::SizePushGen::HORIZONTAL_DIAGRAM);
	m_np_gas_control_widget   = new gs::SizePushGen(gas_control,   "Gas", 	0,  100.,  0.);
	m_np_yaw_control_widget   = new gs::SizePushGen(yaw_control,   "Yaw", 	0,  100., 50., gs::SizePushGen::HORIZONTAL_DIAGRAM);

	// the SA Pilot Chooser buttons buttons
	QPushButton *no_pilot_button = new QPushButton("&Activate No Pilot");
	QPushButton *wp_pilot_button = new QPushButton("&Activate Waypoint Pilot");
	QPushButton *sa_pilot_button = new QPushButton("&Activate SA Pilot");

	// the waypoint pilot airspeed chooser
	m_airspeed_value = new QDoubleSpinBox();
	QCheckBox* use_airspeed_cb = new QCheckBox("Use Airspeed:");

	// the calibration button
	QPushButton* calibration_button = new QPushButton("&Calibrate");

	m_keyboard_grabber = new KeyboardGetterWidget(m_sa_tilt_control_widget, m_sa_roll_control_widget);

	//
	// create the window itself and organize it
	//
	QWidget *main_widget = new QWidget;


	// the SA controllers
	QGridLayout* sa_controls_layout = new QGridLayout;
	sa_controls_layout->addWidget(m_sa_tilt_control_widget, 0, 0, 2, 1);
	sa_controls_layout->addWidget(m_sa_roll_control_widget, 0, 1);
	sa_controls_layout->addWidget(m_sa_yaw_control_widget, 1, 1);
	sa_controls_layout->addWidget(m_sa_gas_control_widget, 0, 2, 2, 1);
	QGroupBox* sa_controls = new QGroupBox(tr("SA Pilot Controls"));
	sa_controls->setLayout(sa_controls_layout);

	// the No Pilot controllers
	QGridLayout* np_controls_layout = new QGridLayout;
	np_controls_layout->addWidget(m_np_tilt_control_widget, 0, 0, 2, 1);
	np_controls_layout->addWidget(m_np_roll_control_widget, 0, 1);
	np_controls_layout->addWidget(m_np_yaw_control_widget, 1, 1);
	np_controls_layout->addWidget(m_np_gas_control_widget, 0, 2, 2, 1);
	QGroupBox* np_controls = new QGroupBox(tr("No Pilot Controls"));
	np_controls->setLayout(np_controls_layout);

	// the sa pilot area
	QGridLayout* sa_area_layout = new QGridLayout;
	sa_area_layout->addWidget(sa_pilot_button, 0, 0);
	sa_area_layout->addWidget(sa_controls, 0, 1);
	QWidget* sa_area = new QWidget;
	sa_area->setLayout(sa_area_layout);

	// the waypoint pilot area
	QGridLayout* wp_area_layout = new QGridLayout;
	wp_area_layout->addWidget(wp_pilot_button, 		0, 0);
	wp_area_layout->addWidget(gen_waypoints_alt, 	0, 1);
	wp_area_layout->addWidget(view_gas, 			0, 2);
	wp_area_layout->addWidget(use_airspeed_cb, 		1, 0);
	wp_area_layout->addWidget(m_airspeed_value, 	1, 1);
	QWidget* wp_area = new QWidget;
	wp_area->setLayout(wp_area_layout);

	// the no pilot area
	QGridLayout* np_area_layout = new QGridLayout;
	np_area_layout->addWidget(no_pilot_button, 0, 0);
	np_area_layout->addWidget(np_controls, 0, 1);
	QWidget* np_area = new QWidget;
	np_area->setLayout(np_area_layout);

	// the pilot's area
	QTabWidget* pilot_widget = new QTabWidget;
	pilot_widget->addTab(np_area, "&No Pilot");
	pilot_widget->addTab(sa_area, "&Stability Augmenting");
	pilot_widget->addTab(wp_area, "&Waypoint Pilot");

	// left down
	QWidget* left_down = new QWidget();
	QGridLayout* left_down_layout = new QGridLayout();
	left_down_layout->addWidget(view_alt, 			0, 0, 2, 1);
	left_down_layout->addWidget(view_link_quality, 	0, 1, 1, 1);
	left_down_layout->addWidget(view_fps, 			1, 1, 1, 1);
	left_down_layout->addWidget(calibration_button, 2, 0, 1, 2);
	left_down_layout->addWidget(pilot_widget, 		0, 2, 3, 1);
	left_down->setLayout(left_down_layout);

	// left up
	QWidget* left_up = new QWidget();
	QGridLayout* left_up_layout = new QGridLayout();
	QTabWidget* orientation_widget = new QTabWidget();
	orientation_widget->addTab(view3d, "3D orientation view");
	orientation_widget->addTab(view2d, "2D orientation view");
	left_up_layout->addWidget(orientation_widget,	0, 0, 1, 4);
	left_up_layout->addWidget(pitch_view, 			1, 0);
	left_up_layout->addWidget(roll_view, 			1, 1);
	left_up_layout->addWidget(yaw_view, 			1, 2);
	left_up_layout->addWidget(view_bat, 			1, 3);
	left_up_layout->addWidget(view_reliability, 	0, 4);
	left_up_layout->addWidget(view_airspeed, 		0, 5);
	left_up->setLayout(left_up_layout);

	// left side
	QWidget* left = new QWidget;
	QVBoxLayout* left_layout = new QVBoxLayout();
	left_layout->addWidget(left_up);
	left_layout->addWidget(left_down);
	left->setLayout(left_layout);

	// alltogether
	QHBoxLayout* main_layout = new QHBoxLayout();
	main_layout->addWidget(left);
	main_layout->addWidget(map_view);
	main_layout->addWidget(m_keyboard_grabber);

	main_widget->setLayout(main_layout);
	wnd->setCentralWidget(main_widget);
	wnd->show();
	view3d->start();
	pitch_view->start();
	roll_view->start();
	yaw_view->start();
	view_reliability->start();
	view_airspeed->start();
	view_fps->start();
	view_link_quality->start();
	view_alt->start();
	view_bat->start();
	view_gas->start();

	// connect signals
	connect(wp_pilot_button, SIGNAL(clicked()), this, SLOT(to_waypoint_pilot()));
	connect(sa_pilot_button, SIGNAL(clicked()), this, SLOT(to_sa_pilot()));
	connect(no_pilot_button, SIGNAL(clicked()), this, SLOT(to_no_pilot()));
	connect(calibration_button, SIGNAL(clicked()), this, SLOT(calibrate()));
	connect(use_airspeed_cb, SIGNAL(toggled(bool)), this, SLOT(use_airspeed(bool)));
	connect(map_view, SIGNAL(got_point(const QgsPoint&, Qt::MouseButton)),
			this, SLOT(got_waypoint(const QgsPoint&, Qt::MouseButton)));
}

void GroundStation::run() {
	m_app.exec();
};

void GroundStation::got_waypoint(const QgsPoint& geo_waypoint, Qt::MouseButton button) {
	std::cout << "GS: Got point!" << std::endl;
	boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();

	lin_algebra::vec2f wp = lin_algebra::create_vec2f(geo_waypoint.x(), geo_waypoint.y());
	std::stringstream wp_str;
	wp_str << wp << m_wanted_alt->get_data();

	conn->write(commands::NAVIGATE_TO);
	conn->write(wp_str.str());
}

GroundStation::~GroundStation() {}

void GroundStation::to_waypoint_pilot() {
	std::cout << "GS: Waypoint pilot activated!" << std::endl;
	boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();
	conn->write(commands::SWITCH_TO_WAYPOINT_PILOT);

	m_keyboard_grabber->releaseKeyboard();
}

void GroundStation::to_sa_pilot() {
	std::cout << "GS: SA pilot activated!" << std::endl;
	boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();
	conn->write(commands::SWITCH_TO_SA_PILOT);
	conn->read();

	m_sa_gas_control_widget->refresh();
	m_sa_yaw_control_widget->refresh();
	m_sa_tilt_control_widget->refresh();
	m_sa_roll_control_widget->refresh();

	m_keyboard_grabber->grabKeyboard();
}

void GroundStation::to_no_pilot() {
	std::cout << "GS: NO pilot activated!" << std::endl;
	boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();
	conn->write(commands::SWITCH_TO_NO_PILOT);
	conn->read();

	// update the data
	m_np_gas_control_widget->refresh();
	m_np_yaw_control_widget->refresh();
	m_np_tilt_control_widget->refresh();
	m_np_roll_control_widget->refresh();

	m_keyboard_grabber->releaseKeyboard();
}

void GroundStation::use_airspeed(bool use) {
	boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();
	if (use) {
		float wanted_airspeed = m_airspeed_value->value() / 3.6f; // from km/h to m/s
		std::cout << "Using airspeed" << wanted_airspeed << std::endl;
		std::stringstream ss;
		conn->write(commands::USE_AIRSPEED);
		conn->write(boost::lexical_cast<std::string>(wanted_airspeed));
	} else {
		std::cout << "Stopped using airspeed" << std::endl;
		conn->write(commands::DONT_USE_AIRSPEED);
	}
}

void GroundStation::calibrate() {
	std::cout << "GS: Calibrating plain!" << std::endl;
	boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();
	conn->write(commands::CALIBRATE);
}
