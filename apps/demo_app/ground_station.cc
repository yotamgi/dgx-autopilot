#include "ground_station.h"
#include "command_protocol.h"

#include <stream/util/udpip_connection.h>
#include <gs/3d_stream_view.h>
#include <gs/size_stream_view.h>
#include <gs/map_stream_view.h>
#include <gs/size_push_gen.h>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>
#include <qt4/Qt/qboxlayout.h>
#include <qt4/Qt/qgridlayout.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

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
			m_pitch->set_value(100.);
			break;
		case Qt::Key_Up:
			m_last_pitch = m_pitch->get_value();
			m_pitch->set_value(0.);
			break;
		case Qt::Key_Right:
			m_last_roll = m_roll->get_value();
			m_roll->set_value(100.);
			break;
		case Qt::Key_Left:
			m_last_roll = m_roll->get_value();
			m_roll->set_value(0.);
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
	boost::shared_ptr<float_recv_stream> reliability = 				boost::make_shared<float_recv_stream>();
	boost::shared_ptr<float_recv_stream> gyro_fps = 				boost::make_shared<float_recv_stream>();
	boost::shared_ptr<vec2_recv_stream> position = 					boost::make_shared<vec2_recv_stream>();
	boost::shared_ptr<float_recv_stream> alt = 						boost::make_shared<float_recv_stream>();
	boost::shared_ptr<float_send_stream> roll_control = 			boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> tilt_control = 			boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> gas_control = 				boost::make_shared<float_send_stream>();
	boost::shared_ptr<float_send_stream> yaw_control = 				boost::make_shared<float_send_stream>();

	stream::AsyncStreamConnection::send_streams_t send_streams = boost::assign::list_of
		((send_stream_ptr)roll_control		)
		((send_stream_ptr)tilt_control		)
		((send_stream_ptr)gas_control		)
		((send_stream_ptr)yaw_control		);


	// fill them into the recv_stream list
	stream::AsyncStreamConnection::recv_streams_t recv_streams = boost::assign::list_of
			((recv_stream_ptr)watch_compass_sensor   )
			((recv_stream_ptr)watch_acc_sensor       )
			((recv_stream_ptr)gyro_watch_orientation )
			((recv_stream_ptr)watch_rest_orientation )
			((recv_stream_ptr)orientation            )
	        ((recv_stream_ptr)reliability            )
	        ((recv_stream_ptr)gyro_fps               )
            ((recv_stream_ptr)position             	 )
			((recv_stream_ptr)alt               	 );

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

	// the reliable stream
	gs::SizeStreamView *view_reliability = new gs::SizeStreamView(reliability, "Reliability", view_update_time, 0., 1.);

	// the gyro_fps stream
	gs::SizeStreamView *view_fps = new gs::SizeStreamView(gyro_fps, "FPS", 1., 0., 400.);

	// the position
	const QSize map_dimention(frame_size.width()/2, frame_size.height());
	gs::MapStreamView* map_view = new gs::MapStreamView(position, 1.0f, map_dimention,
					std::string("../../projs/ground_station/data/map"));

	// the link quality
	gs::SizeStreamView* view_link_quality = new gs::SizeStreamView(
			m_connection->get_quality_stream(), "Link", 0.1f , 0., 1.);

	// the plane's alt
	gs::SizeStreamView* view_alt = new gs::SizeStreamView(alt, "Alt", 0.1f , 0., 200.);

	// the new waypoint's alt
	gs::SizePushGen* gen_waypoints_alt = new gs::SizePushGen(m_wanted_alt, "WP Alt", 0, 200., 100.);

	// the controllers
	gs::SizePushGen* sa_tilt_control  = new gs::SizePushGen(tilt_control, "SA Tilt", 0, 100., 50.);
	gs::SizePushGen* sa_roll_control = new gs::SizePushGen(roll_control, "SA Roll", 0, 100., 50., gs::SizePushGen::HORIZONTAL_DIAGRAM);
	gs::SizePushGen* sa_gas_control = new gs::SizePushGen(gas_control, "SA Gas", 0, 100., 50.);
	gs::SizePushGen* sa_yaw_control  = new gs::SizePushGen(yaw_control, "SA Yaw", 0, 100., 50., gs::SizePushGen::HORIZONTAL_DIAGRAM);

	// the radio buttons
	QRadioButton *wp_pilot_button = new QRadioButton("&Waypoint Pilot");
	QRadioButton *sa_pilot_button = new QRadioButton("&SA Pilot");

	m_keyboard_grabber = new KeyboardGetterWidget(sa_tilt_control, sa_roll_control);

	//
	// create the window itself and organize it
	//
	QWidget *main_widget = new QWidget;

	// the pilot chooser
	QGridLayout* pilot_chooser_layout = new QGridLayout;
	pilot_chooser_layout->addWidget(wp_pilot_button, 0, 0, 1, 2);
	pilot_chooser_layout->addWidget(sa_pilot_button, 1, 0);
	pilot_chooser_layout->addWidget(view_link_quality, 2, 0);
	pilot_chooser_layout->addWidget(view_fps, 2, 1);
	QGroupBox* pilot_chooser = new QGroupBox(tr("Pilot Chooser"));
	pilot_chooser->setLayout(pilot_chooser_layout);

	// the SA controllers
	QGridLayout* sa_controls_layout = new QGridLayout;
	sa_controls_layout->addWidget(sa_tilt_control, 0, 0, 2, 1);
	sa_controls_layout->addWidget(sa_roll_control, 0, 1);
	sa_controls_layout->addWidget(sa_yaw_control, 1, 1);
	sa_controls_layout->addWidget(sa_gas_control, 0, 2, 2, 1);
	QGroupBox* sa_controls = new QGroupBox(tr("SA Pilot Controls"));
	sa_controls->setLayout(sa_controls_layout);

	// left down
	QWidget* left_down = new QWidget();
	QHBoxLayout* left_down_layout = new QHBoxLayout();
	left_down_layout->addWidget(view_alt);
	left_down_layout->addWidget(pilot_chooser);
	left_down_layout->addWidget(sa_controls);
	left_down_layout->addWidget(gen_waypoints_alt);
	left_down->setLayout(left_down_layout);

	// left up
	QWidget* left_up = new QWidget();
	QHBoxLayout* left_up_layout = new QHBoxLayout();
	left_up_layout->addWidget(view3d);
	left_up_layout->addWidget(view_reliability);
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
	view_reliability->start();
	view_fps->start();
	view_link_quality->start();
	view_alt->start();

	// connect signals
	connect(wp_pilot_button, SIGNAL(toggled(bool)), this, SLOT(to_waypoint_pilot(bool)));
	connect(sa_pilot_button, SIGNAL(toggled(bool)), this, SLOT(to_sa_pilot(bool)));
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

void GroundStation::to_waypoint_pilot(bool activate) {
	if (activate) {
		std::cout << "GS: Waypoint pilot activated!" << std::endl;
		boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();
		conn->write(commands::SWITCH_TO_WAYPOINT_PILOT);

		m_keyboard_grabber->releaseKeyboard();
	}
}

void GroundStation::to_sa_pilot(bool activate) {
	if (activate) {
		std::cout << "GS: SA pilot activated!" << std::endl;
		boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();
		conn->write(commands::SWITCH_TO_SA_PILOT);

		m_keyboard_grabber->grabKeyboard();
	}
}
