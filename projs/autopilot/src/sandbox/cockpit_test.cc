#include <stream/stream_connection.h>
#include <stream/filters/stream_recorder.h>
#include <stream/util/tcpip_connection.h>
#include <stream/util/stream_player.h>
#include <platform/dgx1_platform.h>
#include <cockpit.h>
#include <gs/3d_stream_view.h>
#include <gs/size_stream_view.h>
#include <gs/map_stream_view.h>
#include <boost/make_shared.hpp>

#include <string>
#include <iostream>
#include <fstream>

typedef stream::filters::WatchFilter<lin_algebra::vec3f> vec3_watch_stream;

void usage(std::string arg0) {
	std::cout << arg0 << " --sensor-sim <address>  [--present-from <address> ]"
			"[--record <folder> ] [--play <folder> ] [--help]" << std::endl;
}

void update_cockpit(autopilot::Cockpit* cockpit) {
	while (true) {
		cockpit->orientation()->get_data();
	}
}

template <typename T>
boost::shared_ptr<stream::PushGenerator<T> > add_push_recorder(
		boost::shared_ptr<stream::PushGenerator<T> > unfiltered,
		std::ostream& out)
{
	// create a forwarder
	boost::shared_ptr<stream::PushForwarder<T> > forwarder(new stream::PushForwarder<T>);

	// bind it through a filter to the generator
	unfiltered->set_receiver(
			boost::make_shared<stream::filters::RecorderPushFilter<T> >(
					boost::ref(out),
					forwarder
			)
	);

	// return the forwarder
	return forwarder;
}

int main(int argc, char** argv) {
	// global parameters
	const QSize stream3d_dimention(640, 480);
	const float view_update_time = 0.01;

	// commandline parsing
	bool sim = false;
	std::string record_dir;
	std::string play_dir;
	std::string sim_addr;
	std::string present_addr;
	if (argc > 2) {
		for (size_t i=1; i<(size_t)argc; i++) {

			if (std::string(argv[i]) == "--sensor-sim") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(1);
				} else {
					sim_addr = argv[i+1];
					i++;
				}
				sim = true;
				continue;
			}
			if (std::string(argv[i]) == "--record") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(1);
				} else {
					record_dir = argv[i+1];
					i++;
				}
				continue;
			}
			if (std::string(argv[i]) == "--play") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(1);
				} else {
					play_dir = argv[i+1];
					i++;
				}
				continue;
			}
			else if (std::string(argv[i]) == "--present-from") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(2);
				} else {
					present_addr = argv[i+1];
					i++;
				}
				continue;
			}
			else {
				usage(argv[0]);
				exit(1);
			}

		}
	} else if (argc != 1) {
		usage(argv[0]);
		exit(1);
	}

	if (play_dir != "") {
		typedef stream::PopStreamPlayer<lin_algebra::vec3f> vec3_pop_player;
		typedef stream::PushStreamPlayer<lin_algebra::vec3f> vec3_push_player;
		typedef stream::PushStreamPlayer<float> float_push_player;

		std::ifstream acc_file((play_dir + "/acc.stream").c_str());
		std::ifstream compass_file((play_dir + "/compass.stream").c_str());
		std::ifstream gyro_file((play_dir + "/gyro.stream").c_str());
		std::ifstream gps_pos_file((play_dir + "/gps_pos.stream").c_str());
		std::ifstream gps_speed_mag_file((play_dir + "/gps_speed_mag.stream").c_str());
		std::ifstream gps_speed_dir_file((play_dir + "/gps_speed_dir.stream").c_str());

		// create the player platform
		autopilot::NormalPlainPlatform platform;
		platform.acc_sensor = boost::make_shared<vec3_pop_player>(boost::ref(acc_file));
		platform.gyro_sensor = boost::make_shared<vec3_pop_player>(boost::ref(gyro_file));
		platform.compass_sensor = boost::make_shared<vec3_pop_player>(boost::ref(compass_file));
		platform.gps_pos_generator = boost::make_shared<vec3_push_player>(boost::ref(gps_pos_file));
		platform.gps_speed_dir_generator = boost::make_shared<float_push_player>(boost::ref(gps_speed_dir_file));
		platform.gps_speed_mag_generator = boost::make_shared<float_push_player>(boost::ref(gps_speed_mag_file));

		std::cout << "Playing data from directory: " << play_dir << std::endl;

		// add a watch stream on the compass stream
		boost::shared_ptr<vec3_watch_stream> compass_watch(new vec3_watch_stream(platform.compass_sensor));
		platform.compass_sensor = compass_watch;

		autopilot::Cockpit cockpit(platform);
		boost::thread update_thread(update_cockpit, &cockpit);
		QApplication app(argc, argv);
		gs::StreamView3d view3d(view_update_time, stream3d_dimention);

		// the left one
		view3d.addAngleStream(cockpit.orientation()->get_watch_stream(), irr::core::vector3df(-20., 0., 0.));

		// the mid one
		view3d.addAngleStream(cockpit.watch_rest_orientation(), irr::core::vector3df(0., 0., 0.));
		view3d.addVecStream(cockpit.watch_fixed_acc(), irr::core::vector3df(0., 0., 0.));
		view3d.addVecStream(compass_watch->get_watch_stream(), irr::core::vector3df(0., 0., 0.));

		// the right one
		view3d.addAngleStream(cockpit.watch_gyro_orientation(), irr::core::vector3df(20., 0., 0.));

		// the reliable stream
		gs::SizeStreamView view_size(cockpit.watch_rest_reliability(), view_update_time, 0., 1.);

		// the position
		gs::MapStreamView map_view(cockpit.position(), 1.0f, stream3d_dimention,
						std::string("../ground_station/data/map"));

		// create the window itself
		QHBoxLayout* layout = new QHBoxLayout();
		layout->addWidget(&view3d);
		layout->addWidget(&view_size);
		layout->addWidget(&map_view);
		QWidget* wnd = new QWidget;
		wnd->setLayout(layout);
		wnd->show();
		view3d.start();
		view_size.start();
		app.exec();

	} if (present_addr != "")  {
		std::cout << "Presenting from addr " << present_addr << std::endl;
		QApplication app(argc, argv);
		gs::StreamView3d view3d(view_update_time, stream3d_dimention);

		boost::shared_ptr<stream::TcpipServer> server = boost::make_shared<stream::TcpipServer>(present_addr, 0x6060);
		stream::StreamConnection conn(server);

		typedef stream::DataPopStream<lin_algebra::vec3f> vs;
		typedef stream::DataPopStream<lin_algebra::vec2f> v2s;
		typedef stream::DataPopStream<float> fs;

		// the left one
		view3d.addAngleStream(conn.import_pop_stream<vs::type>("orientation"), irr::core::vector3df(-20., 0., 0.));

		// the mid one
		view3d.addAngleStream(conn.import_pop_stream<vs::type>("watch_rest_orientation"), irr::core::vector3df(0., 0., 0.));
		view3d.addVecStream(conn.import_pop_stream<vs::type>("watch_acc_sensor"), irr::core::vector3df(0., 0., 0.));
		view3d.addVecStream(conn.import_pop_stream<vs::type>("watch_compass_sensor"), irr::core::vector3df(0., 0., 0.));

		// the right one
		view3d.addAngleStream(conn.import_pop_stream<vs::type>("gyro_watch_orientation"), irr::core::vector3df(20., 0., 0.));

		// the reliable stream
		gs::SizeStreamView view_size(conn.import_pop_stream<fs::type>("reliability"), view_update_time, 0., 1.);

		// the position
		gs::MapStreamView map_view(conn.import_pop_stream<v2s::type>("position"), 1.0f, stream3d_dimention,
				std::string("../ground_station/data/map"));

		// create the window itself
		QHBoxLayout* layout = new QHBoxLayout();
		layout->addWidget(&view3d);
		layout->addWidget(&view_size);
		layout->addWidget(&map_view);
		QWidget* wnd = new QWidget;
		wnd->setLayout(layout);
		wnd->show();
		view3d.start();
		view_size.start();
		app.exec();

	} else {

		std::cout << "Opening GS" << std::endl;

		autopilot::NormalPlainPlatform platform =
			sim?
				autopilot::create_dgx1_simulator_platform(boost::make_shared<stream::TcpipServer>(sim_addr, 0x6060)):
				autopilot::create_dgx1_platform();

		// add a watch stream on the compass stream
		boost::shared_ptr<vec3_watch_stream> compass_watch(new vec3_watch_stream(platform.compass_sensor));
		platform.compass_sensor = compass_watch;

		std::ofstream acc_file((record_dir + "/acc.stream").c_str());
		std::ofstream compass_file((record_dir + "/compass.stream").c_str());
		std::ofstream gyro_file((record_dir + "/gyro.stream").c_str());
		std::ofstream gps_pos_file((record_dir + "/gps_pos.stream").c_str());
		std::ofstream gps_speed_mag_file((record_dir + "/gps_speed_mag.stream").c_str());
		std::ofstream gps_speed_dir_file((record_dir + "/gps_speed_dir.stream").c_str());
		if (record_dir != "") {
			typedef stream::filters::RecorderPopFilter<lin_algebra::vec3f> pop_recorder;

			platform.acc_sensor.reset(new pop_recorder(acc_file, platform.acc_sensor));
			platform.compass_sensor.reset(new pop_recorder(compass_file, platform.compass_sensor));
			platform.gyro_sensor.reset(new pop_recorder(gyro_file, platform.gyro_sensor));

			platform.gps_pos_generator = add_push_recorder<lin_algebra::vec3f>(platform.gps_pos_generator, gps_pos_file);
			platform.gps_speed_mag_generator = add_push_recorder<float>(platform.gps_speed_mag_generator, gps_speed_mag_file);
			platform.gps_speed_dir_generator = add_push_recorder<float>(platform.gps_speed_dir_generator, gps_speed_dir_file);
  		}

		autopilot::Cockpit cockpit(platform);
		boost::thread update_thread(update_cockpit, &cockpit);

		QApplication app(argc, argv);
		gs::StreamView3d view3d(view_update_time, stream3d_dimention);

		// the left one
		view3d.addAngleStream(cockpit.orientation()->get_watch_stream(), irr::core::vector3df(-20., 0., 0.));

		// the mid one
		view3d.addAngleStream(cockpit.watch_rest_orientation(), irr::core::vector3df(0., 0., 0.));
		view3d.addVecStream(cockpit.watch_fixed_acc(), irr::core::vector3df(0., 0., 0.));
		view3d.addVecStream(compass_watch->get_watch_stream(), irr::core::vector3df(0., 0., 0.));

		// the right one
		view3d.addAngleStream(cockpit.watch_gyro_orientation(), irr::core::vector3df(20., 0., 0.));

		// the reliable stream
		gs::SizeStreamView view_size(cockpit.watch_rest_reliability(), view_update_time, 0., 1.);

		// the position
		gs::MapStreamView map_view(cockpit.position(), 1.0f, stream3d_dimention,
						std::string("../ground_station/data/map"));


		// create the window itself
		QHBoxLayout* layout = new QHBoxLayout();
		layout->addWidget(&view3d);
		layout->addWidget(&view_size);
		layout->addWidget(&map_view);
		QWidget* wnd = new QWidget;
		wnd->setLayout(layout);
		wnd->show();
		view3d.start();
		view_size.start();
		app.exec();

	}
	return 0;

}
