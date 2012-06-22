#include "cockpit_player.h"

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
#include <qt4/Qt/qboxlayout.h>
#include <qt4/Qt/qgridlayout.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>

int stam;

CockpitPlayer::CockpitPlayer(std::string play_dir):
			m_app(stam, (char**)0)
{
	std::cout << "Playing dir " << play_dir << std::endl;

    if (!boost::filesystem::exists(play_dir)) {
            throw std::runtime_error("The requested play dir does not exist");
    }

    // create the stream files
    typedef boost::shared_ptr<std::ifstream> ifile_ptr;
    ifile_ptr acc_file              = boost::make_shared<std::ifstream>((play_dir + "/acc.stream").c_str());
    ifile_ptr compass_file          = boost::make_shared<std::ifstream>((play_dir + "/compass.stream").c_str());
    ifile_ptr gyro_file             = boost::make_shared<std::ifstream>((play_dir + "/gyro.stream").c_str());
    ifile_ptr gps_pos_file          = boost::make_shared<std::ifstream>((play_dir + "/gps_pos.stream").c_str());
    ifile_ptr gps_speed_mag_file    = boost::make_shared<std::ifstream>((play_dir + "/gps_speed_mag.stream").c_str());
    ifile_ptr gps_speed_dir_file    = boost::make_shared<std::ifstream>((play_dir + "/gps_speed_dir.stream").c_str());

    // create the stream players
    m_acc_sensor_player = boost::make_shared<vec3_pop_player>(acc_file);
    m_gyro_sensor_player = boost::make_shared<vec3_pop_player>(gyro_file);
    m_compass_sensor_player = boost::make_shared<vec3_pop_player>(compass_file);
    m_gps_pos_generator_player = boost::make_shared<vec3_push_player>(gps_pos_file);
    m_gps_speed_dir_generator_player = boost::make_shared<float_push_player>(gps_speed_dir_file);
    m_gps_speed_mag_generator_player = boost::make_shared<float_push_player>(gps_speed_mag_file);

    // create the player platform
    autopilot::NormalPlainPlatform platform;
    platform.acc_sensor = 				m_acc_sensor_player;
    platform.gyro_sensor =              m_gyro_sensor_player;
    platform.compass_sensor =           m_compass_sensor_player;
    platform.gps_pos_generator =        m_gps_pos_generator_player;
    platform.gps_speed_dir_generator =  m_gps_speed_dir_generator_player;
    platform.gps_speed_mag_generator =  m_gps_speed_mag_generator_player;

    start_players();

    m_cockpit = boost::make_shared<autopilot::Cockpit>(platform);

    pause_players();


	//
	// Create the window
	//

	QMainWindow* wnd = new QMainWindow;
	const QSize frame_size = QApplication::desktop()->size()*0.80;
	//wnd->setWindowState(Qt::WindowMinimized);

	// global parameters
	const QSize stream3d_dimention(frame_size.width()/2.05, frame_size.height()/2.);
	const float view_update_time = 0.01;

	// the 3d orientation view
	gs::StreamView3d *view3d = new gs::StreamView3d(view_update_time, stream3d_dimention);

	// the right one
    view3d->addAngleStream(m_cockpit->orientation(), irr::core::vector3df(-20., 0., 0.));

    // the mid one
    view3d->addAngleStream(m_cockpit->watch_rest_orientation(), irr::core::vector3df(0., 0., 0.));
    view3d->addVecStream(m_cockpit->watch_fixed_acc(), irr::core::vector3df(0., 0., 0.));

    // the right one
    view3d->addAngleStream(m_cockpit->watch_gyro_orientation(), irr::core::vector3df(20., 0., 0.));

	// the 2d orientation view
	gs::OrientationStreamView* view2d = new gs::OrientationStreamView(m_cockpit->orientation(),
			view_update_time, stream3d_dimention.width());

	// the label orientation view
	gs::LabelStreamView<float>* pitch_view = new gs::LabelStreamView<float>(
			stream::create_func_pop_filter<lin_algebra::vec3f, float>(m_cockpit->orientation(), lin_algebra::get<0>),
			view_update_time,
			"Pitch");
	gs::LabelStreamView<float>* roll_view = new gs::LabelStreamView<float>(
			stream::create_func_pop_filter<lin_algebra::vec3f, float>(m_cockpit->orientation(), lin_algebra::get<2>),
			view_update_time,
			"Roll");
	gs::LabelStreamView<float>* yaw_view = new gs::LabelStreamView<float>(
			stream::create_func_pop_filter<lin_algebra::vec3f, float>(m_cockpit->orientation(), lin_algebra::get<1>),
			view_update_time,
			"Yaw");

	// the control toolbar
	QToolBar* control_toolbar = wnd->addToolBar(tr("Play Control"));
	m_play_action = new QAction(QIcon("/usr/share/icons/oxygen/48x48/actions/media-playback-start.png"), tr("&Play"), control_toolbar);
	QAction* pause_action = new QAction(QIcon("/usr/share/icons/oxygen/48x48/actions/media-playback-pause.png"), tr("&Pause"), control_toolbar);
	QAction* stop_action = new QAction(QIcon("/usr/share/icons/oxygen/48x48/actions/media-playback-stop.png"), tr("&Stop"), control_toolbar);
	m_play_action->setShortcut(Qt::Key_Space);
	m_play_action->setCheckable(true);

	control_toolbar->addAction(m_play_action);
	control_toolbar->addAction(pause_action);
	control_toolbar->addAction(stop_action);

//	// the airspeed stream
//	gs::SizeStreamView *view_airspeed = new gs::SizeStreamView(
//			stream::create_func_pop_filter<float, float>(m_cockpit->a, boost::bind(std::multiplies<float>(), _1, 3.6f)),
//			"Airspeed [km/h]", view_update_time, 0., 100.);

	// the reliable stream
	gs::SizeStreamView *view_reliability = new gs::SizeStreamView(m_cockpit->watch_rest_reliability(), "Reliability", view_update_time, 0., 1.);

	// the position
	const QSize map_dimention(frame_size.width()/2.3, frame_size.height()/1.1);
	gs::MapStreamView* map_view = new gs::MapStreamView(m_cockpit->position(), 1.0f, map_dimention,
					std::string("../../projs/ground_station/data/map"));

	// the plane's alt
	gs::SizeStreamView* view_alt = new gs::SizeStreamView(m_cockpit->alt(), "Alt", 0.1f , 0., 200.);

	// the progress bar
	m_progress_slider = new QSlider();
	m_progress_slider->setOrientation(Qt::Horizontal);
	m_progress_slider->setRange(0, PROGRESS_RESOLUTION);
	m_progress_slider->setTracking(false);

	// The progress text
	m_progress_text = new QLabel("Progress");

	//
	// create the window itself and organize it
	//
	QWidget *main_widget = new QWidget;

	// left down
	QWidget* left_down = new QWidget();
	QGridLayout* left_down_layout = new QGridLayout();
	left_down_layout->addWidget(view_alt, 			0, 0, 1, 1);
	//left_down_layout->addWidget(control_toolbar,	1, 0, 1, 1);
	left_down->setLayout(left_down_layout);

	// left up
	QWidget* left_up = new QWidget();
	QGridLayout* left_up_layout = new QGridLayout();
	QTabWidget* orientation_widget = new QTabWidget();
	orientation_widget->addTab(view3d, "3D orientation view");
	orientation_widget->addTab(view2d, "2D orientation view");
	left_up_layout->addWidget(orientation_widget,	0, 0, 1, 3);
	left_up_layout->addWidget(pitch_view, 			1, 0);
	left_up_layout->addWidget(roll_view, 			1, 1);
	left_up_layout->addWidget(yaw_view, 			1, 2);
	left_up_layout->addWidget(view_reliability, 	0, 4);
	//left_up_layout->addWidget(view_airspeed, 		0, 5);
	left_up->setLayout(left_up_layout);

	// left side
	QWidget* left = new QWidget;
	QVBoxLayout* left_layout = new QVBoxLayout();
	left_layout->addWidget(left_up);
	left_layout->addWidget(left_down);
	left->setLayout(left_layout);

	// down
	QWidget* down = new QWidget;
	QGridLayout* down_layout = new QGridLayout;
	down_layout->addWidget(m_progress_slider, 	0, 0, 1, 2);
	down_layout->addWidget(control_toolbar, 	1, 0, 1, 1);
	down_layout->addWidget(m_progress_text, 	1, 1, 1, 1, Qt::AlignRight);
	down->setLayout(down_layout);

	// alltogether
	QGridLayout* main_layout = new QGridLayout();
	main_layout->addWidget(left, 		0, 0);
	main_layout->addWidget(map_view, 	0, 1);
	main_layout->addWidget(down, 		1, 0, 1, 2);

	main_widget->setLayout(main_layout);
	wnd->setCentralWidget(main_widget);
	wnd->show();
	view3d->start();
	pitch_view->start();
	roll_view->start();
	yaw_view->start();
	view_reliability->start();
	//view_airspeed->start();
	view_alt->start();

	// start the worker timer 20 times a second
	startTimer(1000*(1./20));

	connect(m_play_action, SIGNAL(toggled(bool)), this, SLOT(play(bool)));
	connect(pause_action, SIGNAL(triggered()), this, SLOT(pause()));
	connect(stop_action, SIGNAL(triggered()), this, SLOT(stop()));

	connect(m_progress_slider, SIGNAL(valueChanged(int)),
			this, SLOT(seek(int)));
}

void CockpitPlayer::run() {
	m_app.exec();
};

void CockpitPlayer::timerEvent(QTimerEvent *) {
	m_progress_set_data = PROGRESS_RESOLUTION *
			(m_acc_sensor_player->get_pos()/m_acc_sensor_player->get_stream_length());

	if (!m_progress_slider->isSliderDown()) {
		m_progress_slider->setValue(m_progress_set_data);
	}

	std::stringstream ss;
	ss << m_acc_sensor_player->get_pos() << " / " << m_acc_sensor_player->get_stream_length();
	m_progress_text->setText(tr(ss.str().c_str()));
}

void CockpitPlayer::update_cockpit() {
	while (m_play_action->isChecked()) {
		m_cockpit->orientation()->get_data();
	}
}

void CockpitPlayer::play(bool state) {

	if (state) {
		std::cout << "Playing..." << std::endl;
		start_players();

		m_update_cockpit_thread = boost::thread(&CockpitPlayer::update_cockpit, this);
	} else {
		pause();
	}
}

void CockpitPlayer::pause() {
	std::cout << "Pausing..." << std::endl;

	m_play_action->setChecked(false);
	m_update_cockpit_thread.join();

	pause_players();
}

void CockpitPlayer::stop() {
	std::cout << "Stopping..." << std::endl;

	m_play_action->setChecked(false);
	m_update_cockpit_thread.join();

	stop_players();
}

void CockpitPlayer::seek(int pos) {

	if (pos != m_progress_set_data) {
		float seek_t = m_acc_sensor_player->get_stream_length() * (float(pos)/PROGRESS_RESOLUTION);
		std::cout << "Seeking to " << seek_t << "..." << std::endl;

		seek_players(seek_t);
	}
}

void CockpitPlayer::start_players() {
	m_acc_sensor_player->start();
	m_gyro_sensor_player->start();
	m_compass_sensor_player->start();
	m_gps_pos_generator_player->start();
	m_gps_speed_dir_generator_player->start();
	m_gps_speed_mag_generator_player->start();
}
void CockpitPlayer::pause_players() {
	m_acc_sensor_player->pause();
	m_gyro_sensor_player->pause();
	m_compass_sensor_player->pause();
	m_gps_pos_generator_player->pause();
	m_gps_speed_dir_generator_player->pause();
	m_gps_speed_mag_generator_player->pause();
}
void CockpitPlayer::stop_players() {

	m_acc_sensor_player->stop();
	m_gyro_sensor_player->stop();
	m_compass_sensor_player->stop();
	m_gps_pos_generator_player->stop();
	m_gps_speed_dir_generator_player->stop();
	m_gps_speed_mag_generator_player->stop();
}
void CockpitPlayer::seek_players(float seek_t) {
	m_acc_sensor_player->seek(seek_t);
	m_gyro_sensor_player->seek(seek_t);
	m_compass_sensor_player->seek(seek_t);
	m_gps_pos_generator_player->seek(seek_t);
	m_gps_speed_dir_generator_player->seek(seek_t);
	m_gps_speed_mag_generator_player->seek(seek_t);
}
