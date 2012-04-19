#include <stream/stream_connection.h>
#include <stream/async_stream_connection.h>
#include <stream/filters/stream_recorder.h>
#include <stream/filters/fps_filter.h>
#include <stream/util/tcpip_connection.h>
#include <stream/util/udpip_connection.h>
#include <stream/util/stream_player.h>
#include <platform/dgx1_platform.h>
#include <cockpit.h>
#include <gs/3d_stream_view.h>
#include <gs/size_stream_view.h>
#include <gs/map_stream_view.h>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>

#include <string>
#include <iostream>
#include <fstream>

typedef stream::filters::WatchFilter<lin_algebra::vec3f> vec3_watch_stream;

void usage(std::string arg0) {
	std::cout << arg0 << " --sensor-sim <address>  [--present-from <address> ] "
			"[--present-from-udp <address> ] "
			"[--record <folder> ] [--play <folder> ] [--help] " << std::endl;
}

void update_cockpit(autopilot::Cockpit* cockpit) {
	while (true) {
		cockpit->orientation()->get_data();
	}
}

template <typename T>
boost::shared_ptr<stream::PushGenerator<T> > add_push_recorder(
		boost::shared_ptr<stream::PushGenerator<T> > unfiltered,
		boost::shared_ptr<std::ostream> out)
{
	// create a forwarder
	boost::shared_ptr<stream::PushForwarder<T> > forwarder(new stream::PushForwarder<T>);

	// bind it through a filter to the generator
	unfiltered->set_receiver(
			boost::make_shared<stream::filters::RecorderPushFilter<T> >(
					out,
					forwarder
			)
	);

	// return the forwarder
	return forwarder;
}

typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > vec3_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec2f> > vec2_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPopStream<float> > 			  float_pop_stream_ptr;

void open_gs(
		vec3_pop_stream_ptr compass_sensor,
		vec3_pop_stream_ptr acc_sensor,
		vec3_pop_stream_ptr gyro_orientation,
		vec3_pop_stream_ptr rest_orientation,
		vec3_pop_stream_ptr orientation,
		float_pop_stream_ptr reliability,
		float_pop_stream_ptr fps,
		vec2_pop_stream_ptr position
	)
{

	// global parameters
	const QSize stream3d_dimention(640, 480);
	const float view_update_time = 0.01;

	int a=0;
	QApplication app(a, (char**)0);
	gs::StreamView3d view3d(view_update_time, stream3d_dimention);

	// the right one
	view3d.addAngleStream(orientation, irr::core::vector3df(-20., 0., 0.));

	// the mid one
	view3d.addAngleStream(rest_orientation, irr::core::vector3df(0., 0., 0.));
	view3d.addVecStream(acc_sensor, irr::core::vector3df(0., 0., 0.));
	view3d.addVecStream(compass_sensor, irr::core::vector3df(0., 0., 0.));

	// the right one
	view3d.addAngleStream(gyro_orientation, irr::core::vector3df(20., 0., 0.));

	// the reliable stream
	gs::SizeStreamView view_size(reliability, view_update_time, 0., 1.);

	// the gyro_fps stream
	gs::SizeStreamView view_fps(fps, 1., 0., 4000.);

	// the position
	gs::MapStreamView map_view(position, 1.0f, stream3d_dimention,
					std::string("../ground_station/data/map"));


	// create the window itself
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(&view3d);
	layout->addWidget(&view_size);
	if (fps) layout->addWidget(&view_fps);
	layout->addWidget(&map_view);
	QWidget* wnd = new QWidget;
	wnd->setLayout(layout);
	wnd->show();
	view3d.start();
	view_size.start();
	if (fps) view_fps.start();
	app.exec();
}

autopilot::NormalPlainPlatform record_platform(autopilot::NormalPlainPlatform platform, std::string record_dir) {
	if (boost::filesystem::exists(record_dir) && !boost::filesystem::is_directory(record_dir)) {
		throw std::runtime_error("The requested record dir exists and is not a directory");
	}
	if (!boost::filesystem::exists(record_dir)) {
		boost::filesystem::create_directory(record_dir);
	}

	typedef boost::shared_ptr<std::ofstream> file_ptr;
	file_ptr acc_file 			= boost::make_shared<std::ofstream>((record_dir + "/acc.stream").c_str());
	file_ptr compass_file 		= boost::make_shared<std::ofstream>((record_dir + "/compass.stream").c_str());
	file_ptr gyro_file 			= boost::make_shared<std::ofstream>((record_dir + "/gyro.stream").c_str());
	file_ptr gps_pos_file 		= boost::make_shared<std::ofstream>((record_dir + "/gps_pos.stream").c_str());
	file_ptr gps_speed_mag_file = boost::make_shared<std::ofstream>((record_dir + "/gps_speed_mag.stream").c_str());
	file_ptr gps_speed_dir_file = boost::make_shared<std::ofstream>((record_dir + "/gps_speed_dir.stream").c_str());

	typedef stream::filters::RecorderPopFilter<lin_algebra::vec3f> pop_recorder;

	platform.acc_sensor.reset(new pop_recorder(acc_file, platform.acc_sensor));
	platform.compass_sensor.reset(new pop_recorder(compass_file, platform.compass_sensor));
	platform.gyro_sensor.reset(new pop_recorder(gyro_file, platform.gyro_sensor));

	platform.gps_pos_generator = add_push_recorder<lin_algebra::vec3f>(platform.gps_pos_generator, gps_pos_file);
	platform.gps_speed_mag_generator = add_push_recorder<float>(platform.gps_speed_mag_generator, gps_speed_mag_file);
	platform.gps_speed_dir_generator = add_push_recorder<float>(platform.gps_speed_dir_generator, gps_speed_dir_file);

	return platform;
}

autopilot::NormalPlainPlatform platform_player(std::string play_dir) {
	if (!boost::filesystem::exists(play_dir)) {
		throw std::runtime_error("The requested play dir does not exist");
	}
	typedef stream::PopStreamPlayer<lin_algebra::vec3f> vec3_pop_player;
	typedef stream::PushStreamPlayer<lin_algebra::vec3f> vec3_push_player;
	typedef stream::PushStreamPlayer<float> float_push_player;

	typedef boost::shared_ptr<std::ifstream> ifile_ptr;
	ifile_ptr acc_file				= boost::make_shared<std::ifstream>((play_dir + "/acc.stream").c_str());
	ifile_ptr compass_file			= boost::make_shared<std::ifstream>((play_dir + "/compass.stream").c_str());
	ifile_ptr gyro_file				= boost::make_shared<std::ifstream>((play_dir + "/gyro.stream").c_str());
	ifile_ptr gps_pos_file			= boost::make_shared<std::ifstream>((play_dir + "/gps_pos.stream").c_str());
	ifile_ptr gps_speed_mag_file	= boost::make_shared<std::ifstream>((play_dir + "/gps_speed_mag.stream").c_str());
	ifile_ptr gps_speed_dir_file	= boost::make_shared<std::ifstream>((play_dir + "/gps_speed_dir.stream").c_str());

	// create the player platform
	autopilot::NormalPlainPlatform platform;
	platform.acc_sensor = boost::make_shared<vec3_pop_player>(acc_file);
	platform.gyro_sensor = boost::make_shared<vec3_pop_player>(gyro_file);
	platform.compass_sensor = boost::make_shared<vec3_pop_player>(compass_file);
	platform.gps_pos_generator = boost::make_shared<vec3_push_player>(gps_pos_file);
	platform.gps_speed_dir_generator = boost::make_shared<float_push_player>(gps_speed_dir_file);
	platform.gps_speed_mag_generator = boost::make_shared<float_push_player>(gps_speed_mag_file);

	return platform;
}

int main(int argc, char** argv) {
	// commandline parsing
	bool sim = false;
	std::string record_dir;
	std::string play_dir;
	std::string sim_addr;
	std::string present_addr;
	std::string present_addr_udp;
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
			else if (std::string(argv[i]) == "--present-from-udp") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(3);
				} else {
					present_addr_udp = argv[i+1];
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

	// if the user asked to play, play!
	if (play_dir != "") {
		std::cout << "Playing data from directory: " << play_dir << std::endl;
		autopilot::NormalPlainPlatform platform = platform_player(play_dir);

		// add a watch stream on the compass stream
		boost::shared_ptr<vec3_watch_stream> compass_watch(new vec3_watch_stream(platform.compass_sensor));
		platform.compass_sensor = compass_watch;

		// add fps stream to the gyro stream
		boost::shared_ptr<stream::filters::FpsFilter<lin_algebra::vec3f> > fpsed_gyro(
				new stream::filters::FpsFilter<lin_algebra::vec3f>(platform.gyro_sensor)
		);
		platform.gyro_sensor = fpsed_gyro;

		// create the cockpit and run it
		autopilot::Cockpit cockpit(platform);
		boost::thread update_thread(update_cockpit, &cockpit);

		// open the ground station with data
		open_gs(compass_watch->get_watch_stream(),
				cockpit.watch_fixed_acc(),
				cockpit.watch_gyro_orientation(),
				cockpit.watch_rest_orientation(),
				cockpit.orientation()->get_watch_stream(),
				cockpit.watch_rest_reliability(),
				fpsed_gyro->get_fps_stream(),
				cockpit.position()
		);
	}

	// if the user asked to view a foreighn host, do it!
	if (present_addr != "")  {
		std::cout << "Presenting from addr " << present_addr << std::endl;

		// open server and connect to the foreign host
		boost::shared_ptr<stream::TcpipServer> server = boost::make_shared<stream::TcpipServer>(present_addr, 0x6060);
		stream::StreamConnection conn(server);

		// open the ground station with data
		open_gs(
				conn.import_pop_stream<lin_algebra::vec3f>("watch_compass_sensor"),
				conn.import_pop_stream<lin_algebra::vec3f>("watch_acc_sensor"),
				conn.import_pop_stream<lin_algebra::vec3f>("gyro_watch_orientation"),
				conn.import_pop_stream<lin_algebra::vec3f>("watch_rest_orientation"),
				conn.import_pop_stream<lin_algebra::vec3f>("orientation"),
				conn.import_pop_stream<float>("reliability"),
				conn.import_pop_stream<float>("gyro_fps"),
				conn.import_pop_stream<lin_algebra::vec2f>("position")
		);
	}

	// if the user asked to view a foreighn host, do it!
	else if (present_addr_udp != "")  {
		std::cout << "Presenting UDP from addr " << present_addr << std::endl;

		typedef boost::shared_ptr<stream::AsyncStreamConnection::RecvStream> recv_stream_ptr;
		typedef stream::AsyncStreamConnection::RecvPopStream<lin_algebra::vec3f> vec3_recv_stream;
		typedef stream::AsyncStreamConnection::RecvPopStream<lin_algebra::vec2f> vec2_recv_stream;
		typedef stream::AsyncStreamConnection::RecvPopStream<float> float_recv_stream;

		// create the streams
		boost::shared_ptr<vec3_recv_stream> watch_compass_sensor = 		boost::make_shared<vec3_recv_stream>();
		boost::shared_ptr<vec3_recv_stream> watch_acc_sensor = 			boost::make_shared<vec3_recv_stream>();
		boost::shared_ptr<vec3_recv_stream> gyro_watch_orientation =	boost::make_shared<vec3_recv_stream>();
		boost::shared_ptr<vec3_recv_stream> watch_rest_orientation =	boost::make_shared<vec3_recv_stream>();
		boost::shared_ptr<vec3_recv_stream> orientation = 				boost::make_shared<vec3_recv_stream>();
		boost::shared_ptr<float_recv_stream> reliability = 				boost::make_shared<float_recv_stream>();
		boost::shared_ptr<float_recv_stream> gyro_fps = 				boost::make_shared<float_recv_stream>();
		boost::shared_ptr<vec2_recv_stream> posisition = 				boost::make_shared<vec2_recv_stream>();

		// fill them into the recv_stream list
		stream::AsyncStreamConnection::recv_streams_t recv_streams = boost::assign::list_of
				((recv_stream_ptr)watch_compass_sensor   )
				((recv_stream_ptr)watch_acc_sensor       )
				((recv_stream_ptr)gyro_watch_orientation )
				((recv_stream_ptr)watch_rest_orientation )
				((recv_stream_ptr)orientation            )
		        ((recv_stream_ptr)reliability            )
		        ((recv_stream_ptr)gyro_fps               )
                ((recv_stream_ptr)posisition             );

		// creating the udp connection stuff
		boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
				boost::make_shared<stream::UdpipConnectionFactory>(4444, present_addr_udp, 5555);

		// create the async stream connection
		stream::AsyncStreamConnection c(stream::AsyncStreamConnection::send_streams_t(),
										recv_streams,
										conn_factory,
										true,
										50.);
		c.start();

		// open the ground station with the streams
		open_gs(
				watch_compass_sensor,
				watch_acc_sensor,
				gyro_watch_orientation,
				watch_rest_orientation,
				orientation,
				reliability,
				gyro_fps,
				posisition
		);
	}

	// if not, just open a ground station with the asked platform
	else {
		std::cout << "Opening GS" << std::endl;

		// create the platform according to what the user asked
		autopilot::NormalPlainPlatform platform =
			sim?
				autopilot::create_dgx1_simulator_platform(boost::make_shared<stream::TcpipServer>(sim_addr, 0x6060)):
				autopilot::create_dgx1_platform();

		// if the user asked to record, record!
		if (record_dir != "") {
			platform = record_platform(platform, record_dir);
  		}

		// add a watch stream on the compass stream
		boost::shared_ptr<vec3_watch_stream> compass_watch(new vec3_watch_stream(platform.compass_sensor));
		platform.compass_sensor = compass_watch;

		// add fps stream to the gyro stream
		boost::shared_ptr<stream::filters::FpsFilter<lin_algebra::vec3f> > fpsed_gyro(
				new stream::filters::FpsFilter<lin_algebra::vec3f>(platform.gyro_sensor)
		);
		platform.gyro_sensor = fpsed_gyro;

		// create the cockpit and run it
		autopilot::Cockpit cockpit(platform);
		boost::thread update_thread(update_cockpit, &cockpit);

		// open the ground station with data
		open_gs(compass_watch->get_watch_stream(),
				cockpit.watch_fixed_acc(),
				cockpit.watch_gyro_orientation(),
				cockpit.watch_rest_orientation(),
				cockpit.orientation()->get_watch_stream(),
				cockpit.watch_rest_reliability(),
				fpsed_gyro->get_fps_stream(),
				cockpit.position()->get_watch_stream()
		);
	}
	return 0;

}
