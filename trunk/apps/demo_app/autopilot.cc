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
#include <boost/lexical_cast.hpp>
#include "command_protocol.h"

#ifndef DEVICE
#include <simulator/simulator_platform.h>
#include <simulator/platforms.h>
#endif

#include <string>
#include <iostream>
#include <fstream>

typedef stream::filters::WatchFilter<lin_algebra::vec3f> vec3_watch_stream;
typedef stream::filters::WatchFilter<float> 			 float_watch_stream;

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
	unfiltered->register_receiver(
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
	file_ptr airspeed_file		= boost::make_shared<std::ofstream>((record_dir + "/airspeed.stream").c_str());
	file_ptr gps_pos_file 		= boost::make_shared<std::ofstream>((record_dir + "/gps_pos.stream").c_str());
	file_ptr gps_speed_mag_file = boost::make_shared<std::ofstream>((record_dir + "/gps_speed_mag.stream").c_str());
	file_ptr gps_speed_dir_file = boost::make_shared<std::ofstream>((record_dir + "/gps_speed_dir.stream").c_str());
	file_ptr bat_file 			= boost::make_shared<std::ofstream>((record_dir + "/bat.stream").c_str());

	typedef stream::filters::RecorderPopFilter<lin_algebra::vec3f> vec3f_pop_recorder;
	typedef stream::filters::RecorderPopFilter<float> float_pop_recorder;

	platform.acc_sensor.reset(new vec3f_pop_recorder(acc_file, platform.acc_sensor));
	platform.compass_sensor.reset(new vec3f_pop_recorder(compass_file, platform.compass_sensor));
	platform.gyro_sensor.reset(new vec3f_pop_recorder(gyro_file, platform.gyro_sensor));
	platform.airspeed_sensor.reset(new float_pop_recorder(airspeed_file, platform.airspeed_sensor));
	platform.battery_sensor.reset(new float_pop_recorder(bat_file, platform.battery_sensor));

	platform.gps_pos_generator = add_push_recorder<lin_algebra::vec3f>(platform.gps_pos_generator, gps_pos_file);
	platform.gps_speed_mag_generator = add_push_recorder<float>(platform.gps_speed_mag_generator, gps_speed_mag_file);
	platform.gps_speed_dir_generator = add_push_recorder<float>(platform.gps_speed_dir_generator, gps_speed_dir_file);

	return platform;
}

typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > vec3_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec2f> > vec2_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPopStream<float> > 			  float_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPushStream<float> > 			  float_push_stream_ptr;


boost::shared_ptr<stream::AsyncStreamConnection>  export_data(std::string export_addr,
		vec3_pop_stream_ptr compass_sensor,
		vec3_pop_stream_ptr acc_sensor,
		vec3_pop_stream_ptr gyro_orientation,
		vec3_pop_stream_ptr rest_orientation,
		vec3_pop_stream_ptr orientation,
		float_pop_stream_ptr airspeed,
		float_pop_stream_ptr reliability,
		float_pop_stream_ptr fps,
		vec2_pop_stream_ptr position,
		float_pop_stream_ptr alt,
		float_pop_stream_ptr battery,
		float_pop_stream_ptr gas_servo,
		float_push_stream_ptr sa_tilt_control,
		float_push_stream_ptr sa_pitch_control,
		float_push_stream_ptr sa_gas_control,
		float_push_stream_ptr sa_yaw_control,
		float_push_stream_ptr gas_control,
		float_push_stream_ptr yaw_control,
		float_push_stream_ptr roll_control,
		float_push_stream_ptr pitch_control
	)
{
	std::cout << "Exporting all data in UDP" << std::endl;
	typedef boost::shared_ptr<stream::AsyncStreamConnection::SendStream> send_stream_ptr;
	typedef boost::shared_ptr<stream::AsyncStreamConnection::RecvStream> recv_stream_ptr;
	typedef stream::AsyncStreamConnection::SendPopStream<lin_algebra::vec3f> vec3_send_stream;
	typedef stream::AsyncStreamConnection::SendPopStream<lin_algebra::vec2f> vec2_send_stream;
	typedef stream::AsyncStreamConnection::SendPopStream<float> float_send_stream;
	typedef stream::AsyncStreamConnection::RecvPushStream<float> float_recv_stream;

	// create the streams list
	stream::AsyncStreamConnection::send_streams_t send_streams = boost::assign::list_of
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(compass_sensor))
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(acc_sensor))
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(gyro_orientation))
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(rest_orientation))
		((send_stream_ptr)boost::make_shared<vec3_send_stream>(orientation))
		((send_stream_ptr)boost::make_shared<float_send_stream>(airspeed))
		((send_stream_ptr)boost::make_shared<float_send_stream>(reliability))
		((send_stream_ptr)boost::make_shared<float_send_stream>(fps))
		((send_stream_ptr)boost::make_shared<vec2_send_stream>(position))
		((send_stream_ptr)boost::make_shared<float_send_stream>(alt))
		((send_stream_ptr)boost::make_shared<float_send_stream>(battery))
		((send_stream_ptr)boost::make_shared<float_send_stream>(gas_servo));

	stream::AsyncStreamConnection::recv_streams_t recv_streams = boost::assign::list_of
			((recv_stream_ptr)boost::make_shared<float_recv_stream>(sa_tilt_control)	)
			((recv_stream_ptr)boost::make_shared<float_recv_stream>(sa_pitch_control)	)
			((recv_stream_ptr)boost::make_shared<float_recv_stream>(sa_gas_control) 	)
			((recv_stream_ptr)boost::make_shared<float_recv_stream>(sa_yaw_control) 	)
			((recv_stream_ptr)boost::make_shared<float_recv_stream>(gas_control) 		)
			((recv_stream_ptr)boost::make_shared<float_recv_stream>(yaw_control) 		)
			((recv_stream_ptr)boost::make_shared<float_recv_stream>(roll_control) 		)
			((recv_stream_ptr)boost::make_shared<float_recv_stream>(pitch_control) 		);


	// creating the udp connection stuff
	boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
			boost::make_shared<stream::UdpipConnectionFactory>(5555, export_addr, 4444);

	// create the async stream connection
	boost::shared_ptr<stream::AsyncStreamConnection> c =
			boost::make_shared<stream::AsyncStreamConnection>(send_streams,
									recv_streams,
									conn_factory,
									false,
									50.
	);

	return c;
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
		platform = autopilot::create_dgx1_2_platform();
	} else if (platform_type == "sim") {
#ifndef DEVICE
		const lin_algebra::vec3f SHOMRAT_POS = lin_algebra::create_vec3f(35.1043, 32.9432, 0.);
		simulator::WindGen::Params wind_params;
		wind_params.const_wind = lin_algebra::create_vec3f(2., 0, 0);
		wind_params.long_disturbance_strength = 0.4;
		wind_params.little_disturbance_strength = 0.1;

		platform = *simulator::create_simulator_platform(simulator::platforms::dgx_platform, wind_params,
				SHOMRAT_POS);
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

	// add a watch stream on the gas servo
	boost::shared_ptr<float_watch_stream> gas_watch(new float_watch_stream(platform.gas_servo));
	platform.gas_servo = gas_watch;

	// add a watch stream on the compass stream
	boost::shared_ptr<vec3_watch_stream> compass_watch(new vec3_watch_stream(platform.compass_sensor));
	platform.compass_sensor = compass_watch;

	// add fps stream to the gyro stream
	boost::shared_ptr<stream::filters::FpsFilter<lin_algebra::vec3f> > fpsed_gyro(
			new stream::filters::FpsFilter<lin_algebra::vec3f>(platform.gyro_sensor, 0.4f)
	);
	platform.gyro_sensor = fpsed_gyro;

	// create the cockpit
	boost::shared_ptr<autopilot::Cockpit> cockpit = boost::make_shared<autopilot::Cockpit>(platform);

	// configure the wp pilot
	autopilot::WaypointPilot::Params wp_pilot_params;
	wp_pilot_params.max_climbing_angle = 15.;
	wp_pilot_params.max_decending_angle = -10.;
	wp_pilot_params.climbing_gas = 75.;//100.;
	wp_pilot_params.decending_gas = 40.;//20.;
	wp_pilot_params.avg_gas = 60.;//40;
	wp_pilot_params.avg_pitch_angle = 0.;
	wp_pilot_params.max_roll_angle = 40.;
	wp_pilot_params.pitch_severity = 20.;
	wp_pilot_params.roll_severity = 20.;
	wp_pilot_params.heading_fine_tunning = 30.;
	wp_pilot_params.use_airspeed = false;
	wp_pilot_params.avg_airspeed = 10.;
	wp_pilot_params.airspeed_severity = 3.;

	// create the wp pilot
	autopilot::WaypointPilot wp_pilot(wp_pilot_params, cockpit);

	// create the sa pilot
	autopilot::StabilityAugmentingPilot sa_pilot(cockpit, 20., 20.);
	sa_pilot.get_tilt_control()->set_data(60.0f);

	boost::shared_ptr<stream::PushForwarder<float> > sa_gas_control   = boost::make_shared<stream::PushForwarder<float> >();
	boost::shared_ptr<stream::PushForwarder<float> > sa_yaw_control   = boost::make_shared<stream::PushForwarder<float> >();
	boost::shared_ptr<stream::PushForwarder<float> > gas_control   = boost::make_shared<stream::PushForwarder<float> >();
	boost::shared_ptr<stream::PushForwarder<float> > yaw_control   = boost::make_shared<stream::PushForwarder<float> >();
	boost::shared_ptr<stream::PushForwarder<float> > roll_control  = boost::make_shared<stream::PushForwarder<float> >();
	boost::shared_ptr<stream::PushForwarder<float> > pitch_control = boost::make_shared<stream::PushForwarder<float> >();

	// export all the data
	boost::shared_ptr<stream::AsyncStreamConnection> conn = export_data(gs_address,
			compass_watch->get_watch_stream(),
			cockpit->watch_fixed_acc(),
			cockpit->watch_gyro_orientation(),
			cockpit->watch_rest_orientation(),
			cockpit->orientation(),
			cockpit->air_speed(),
			cockpit->watch_rest_reliability(),
			fpsed_gyro->get_fps_stream(),
			cockpit->position(),
			cockpit->alt(),
			cockpit->battery_state(),
			gas_watch->get_watch_stream(),
			sa_pilot.get_roll_control(),
			sa_pilot.get_tilt_control(),
			sa_gas_control,
			sa_yaw_control,
			gas_control,
			yaw_control,
			roll_control,
			pitch_control
	);

	conn->start();

	// start in no pilot mode
	cockpit->run();
	gas_control->register_receiver(cockpit->gas_servo());
	yaw_control->register_receiver(cockpit->yaw_servo());
	roll_control->register_receiver(cockpit->pitch_servo());
	pitch_control->register_receiver(cockpit->tilt_servo());


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

				autopilot::WaypointPilot::waypoint_list path = wp_pilot.get_path();
				path.push_back(autopilot::WaypointPilot::waypoint(wp, wp_alt));
				wp_pilot.set_path(path);
			}

			else if (command == commands::SWITCH_TO_WAYPOINT_PILOT) {
				std::cout << "Moving to Waypoint pilot... ";
				cockpit->stop();
				sa_pilot.stop();

				// unregister the no pilot reciever, if registered
				gas_control->unregister_receiver(cockpit->gas_servo());
				yaw_control->unregister_receiver(cockpit->yaw_servo());
				roll_control->unregister_receiver(cockpit->pitch_servo());
				pitch_control->unregister_receiver(cockpit->tilt_servo());

				// unregister the sa pilot recievers, if registered
				sa_gas_control->unregister_receiver(cockpit->gas_servo());
				sa_yaw_control->unregister_receiver(cockpit->yaw_servo());

				wp_pilot.start();
				std::cout << " Finished." << std::endl;
			}

			else if (command == commands::SWITCH_TO_SA_PILOT) {
				std::cout << "Moving to SA pilot... ";
				cockpit->stop();
				wp_pilot.stop();

				// unregister the no pilot reciever, if registersd
				roll_control->unregister_receiver(cockpit->pitch_servo());
				pitch_control->unregister_receiver(cockpit->tilt_servo());
				gas_control->unregister_receiver(cockpit->gas_servo());
				yaw_control->unregister_receiver(cockpit->yaw_servo());

				// register the sa controls
				sa_gas_control->register_receiver(cockpit->gas_servo());
				sa_yaw_control->register_receiver(cockpit->yaw_servo());

				sa_pilot.start();
				conn->write(commands::OK);
				std::cout << " Finished." << std::endl;
			}

			else if (command == commands::SWITCH_TO_NO_PILOT) {
				std::cout << "Moving to no pilot... ";
				sa_pilot.stop();
				wp_pilot.stop();

				// unregister the sa pilot recievers, if registered
				sa_gas_control->unregister_receiver(cockpit->gas_servo());
				sa_yaw_control->unregister_receiver(cockpit->yaw_servo());

				// register the no pilot controls
				gas_control->register_receiver(cockpit->gas_servo());
				yaw_control->register_receiver(cockpit->yaw_servo());
				roll_control->register_receiver(cockpit->pitch_servo());
				pitch_control->register_receiver(cockpit->tilt_servo());

				cockpit->run();
				conn->write(commands::OK);
				std::cout << " Finished." << std::endl;
			}

			else if (command == commands::CALIBRATE) {
				std::cout << "Calibrating the plain... ";
				cockpit->calibrate();
				std::cout << " Finished." << std::endl;
			}

			else if (command == commands::USE_AIRSPEED) {
				float wanted_airspeed = boost::lexical_cast<float>(conn->read());
				wp_pilot.params().avg_airspeed = wanted_airspeed;
				wp_pilot.params().use_airspeed = true;
				std::cout << "Setting airspeed to " << wanted_airspeed << std::endl;
			}

			else if (command == commands::DONT_USE_AIRSPEED) {
				wp_pilot.params().use_airspeed = false;
				std::cout << "Stopped using airspeed" << std::endl;
			}


		} catch (stream::ConnectionExceptioin e) {
			std::cout << "An exception was thrown : " << e.what() << ". Continuing" << std::endl;
		}
	}

	return 0;

}
