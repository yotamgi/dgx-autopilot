#include <stream/stream_connection.h>
#include <stream/async_stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <stream/util/udpip_connection.h>
#include <stream/filters/stream_recorder.h>
#include <stream/filters/fps_filter.h>
#include <autopilot/platform/dgx1_platform.h>
#include <autopilot/platform/platform_export_import.h>
#include <autopilot/cockpit.h>
#include <autopilot/stability_augmenting_pilot.h>
#include <autopilot/waypoint_pilot.h>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>
#include "command_protocol.h"

#ifndef DEVICE
#include <simulator/simulator_platform.h>
#include <simulator/platforms.h>
#endif

#include <string>
#include <iostream>
#include <fstream>

typedef stream::filters::WatchFilter<lin_algebra::vec3f> vec3_watch_stream;

void usage_and_exit(std::string arg0) {
	std::cout << arg0 << " <gs_address> [--platform hw|sim|ip_addr] [--record <folder> ] [--help] " << std::endl;
	exit(1);
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

typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > vec3_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec2f> > vec2_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPopStream<float> > 			  float_pop_stream_ptr;

boost::shared_ptr<stream::AsyncStreamConnection>  export_data(std::string export_addr,
		vec3_pop_stream_ptr compass_sensor,
		vec3_pop_stream_ptr acc_sensor,
		vec3_pop_stream_ptr gyro_orientation,
		vec3_pop_stream_ptr rest_orientation,
		vec3_pop_stream_ptr orientation,
		float_pop_stream_ptr reliability,
		float_pop_stream_ptr fps,
		vec2_pop_stream_ptr position,
		float_pop_stream_ptr alt
	)
{
	std::cout << "Exporting all data in UDP" << std::endl;
	typedef boost::shared_ptr<stream::AsyncStreamConnection::SendStream> send_stream_ptr;
	typedef stream::AsyncStreamConnection::SendPopStream<lin_algebra::vec3f> vec3_send_stream;
	typedef stream::AsyncStreamConnection::SendPopStream<lin_algebra::vec2f> vec2_send_stream;
	typedef stream::AsyncStreamConnection::SendPopStream<float> float_send_stream;

	// create the streams list
	stream::AsyncStreamConnection::send_streams_t send_streams = boost::assign::list_of
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(compass_sensor))
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(acc_sensor))
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(gyro_orientation))
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(rest_orientation))
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(orientation))
		((send_stream_ptr)boost::make_shared<float_send_stream>(reliability))
		((send_stream_ptr)boost::make_shared<float_send_stream>(fps))
		((send_stream_ptr)boost::make_shared<vec2_send_stream>(position))
		((send_stream_ptr)boost::make_shared<float_send_stream>(alt));


	// creating the udp connection stuff
	boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
			boost::make_shared<stream::UdpipConnectionFactory>(5555, export_addr, 4444);

	// create the async stream connection
	boost::shared_ptr<stream::AsyncStreamConnection> c =
			boost::make_shared<stream::AsyncStreamConnection>(send_streams,
									stream::AsyncStreamConnection::recv_streams_t(),
									conn_factory,
									false,
									50.
	);

	return c;
}

void update_cockpit(autopilot::Cockpit* cockpit) {
	while (true) {
		cockpit->orientation()->get_data();
	}
}

int main(int argc, char** argv) {
	// commandline parsing
	std::string record_dir;
	std::string play_dir;
	std::string platform_type = "hw";
	std::string gs_address;

	if (argc < 2)			usage_and_exit(argv[0]);
	if (argv[1][0] == '-')	usage_and_exit(argv[0]);

	gs_address = argv[1];

	for (size_t i=2; i<(size_t)argc; i++) {

		if (std::string(argv[i]) == "--platform") {
			if ((size_t)argc < i+1) {
				usage_and_exit(argv[0]);
			} else {
				platform_type = argv[i+1];
				i++;
			}
			continue;
		}
		if (std::string(argv[i]) == "--record") {
			if ((size_t)argc < i+1) {
				usage_and_exit(argv[0]);
			} else {
				record_dir = argv[i+1];
				i++;
			}
			continue;
		}
		else {
			usage_and_exit(argv[0]);
		}
	}

	std::cout << "Running Autopilot" << std::endl;

	// create the platform according to what the user asked
	autopilot::NormalPlainPlatform platform;
	if (platform_type == "hw") {
		platform = autopilot::create_dgx1_platform();
	} else if (platform_type == "sim") {
#ifndef DEVICE
		platform = *simulator::create_simulator_platform(simulator::platforms::dgx_platform);
#else
		throw std::runtime_error("Can not open simulator platform on device");
#endif
	} else {
		platform = *autopilot::import_platform(boost::make_shared<stream::TcpipServer>(platform_type , 0x6060));
	}

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

	// create the cockpit
	boost::shared_ptr<autopilot::Cockpit> cockpit = boost::make_shared<autopilot::Cockpit>(platform);

	// configure the pilot
	autopilot::WaypointPilot::Params pilot_params;
	pilot_params.max_tilt_angle = 20.;
	pilot_params.max_pitch_angle = 20.;
	pilot_params.max_climbing_strength = 100.0f;
	pilot_params.climbing_gas = 100.;
	pilot_params.max_decending_strength = 25.0f;
	pilot_params.decending_gas = 20.;
	pilot_params.avg_gas = 40;
	pilot_params.avg_pitch_strength = 50.;

	// create the pilot
	autopilot::WaypointPilot pilot(pilot_params, cockpit);
	pilot.start(true);
//			cockpit->gas_servo()->set_data(45.0f);
//			cockpit->yaw_servo()->set_data(50.0f);

//	autopilot::StabilityAugmentingPilot::Params pilot_params;
//	pilot_params.max_pitch_angle = 15.;
//	pilot_params.max_tilt_angle = 20.;
//
//	// create the pilot
//	autopilot::StabilityAugmentingPilot pilot(pilot_params, cockpit);
//	pilot.get_pitch_control()->set_data(90.0f);
//	pilot.run(true);


	// export all the data
	boost::shared_ptr<stream::AsyncStreamConnection> conn = export_data(gs_address,
			compass_watch->get_watch_stream(),
			cockpit->watch_fixed_acc(),
			cockpit->watch_gyro_orientation(),
			cockpit->watch_rest_orientation(),
			cockpit->orientation()->get_watch_stream(),
			cockpit->watch_rest_reliability(),
			fpsed_gyro->get_fps_stream(),
			cockpit->position(),
			cockpit->alt()
	);

	conn->start();

	stream::TcpipClient control_connection(gs_address, commands::port);
	while (true) {
		try {
			boost::shared_ptr<stream::Connection> conn = control_connection.get_connection();
			const std::string command = conn->read();

			if (command == commands::NAVIGATE_TO) {
				std::stringstream wp_str(conn->read());
				lin_algebra::vec2f wp;
				float wp_alt;
				wp_str >> wp >> wp_alt;

				autopilot::WaypointPilot::waypoint_list path = pilot.get_path();
				path.push_back(autopilot::WaypointPilot::waypoint(wp, wp_alt));
				pilot.set_path(path);
			}
		} catch (stream::ConnectionExceptioin e) {
			std::cout << "An exception was thrown : " << e.what() << ". Continuing" << std::endl;
		}
	}

	return 0;

}
