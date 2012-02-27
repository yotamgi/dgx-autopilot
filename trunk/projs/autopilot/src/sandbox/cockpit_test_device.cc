#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <stream/filters/stream_recorder.h>
#include <stream/filters/fps_filter.h>
#include <stream/util/stream_player.h>
#include <platform/dgx1_platform.h>
#include <cockpit.h>
#include <boost/filesystem.hpp>

#include <string>
#include <iostream>

typedef stream::filters::WatchFilter<lin_algebra::vec3f> vec3_watch_stream;

template <typename T>
void stream_popper(boost::shared_ptr<stream::DataPopStream<T> > stream) {
	while (true) {
		stream->get_data();
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

int main(int argc, char** argv) {
	if (argc != 2 && argc != 4) {
		std::cout << "Please enter the address to export to" << std::endl;
		exit(1);
	}

	std::string record_dir = "";
	if (argc == 4 && std::string(argv[2]) == "--record") {
		record_dir = std::string(argv[3]);
	}

	autopilot::NormalPlainPlatform platform = autopilot::create_dgx1_platform();

	// if the user asked to record, record!
	if (record_dir != "") {
		platform = record_platform(platform, record_dir);
	}

	// add a watch stream on the compass and acc stream
	boost::shared_ptr<vec3_watch_stream> compass_watch(new vec3_watch_stream(platform.compass_sensor));
	platform.compass_sensor = compass_watch;
	boost::shared_ptr<vec3_watch_stream> acc_watch(new vec3_watch_stream(platform.acc_sensor));
	platform.acc_sensor = acc_watch;

	// add fps stream to the gyro stream
	boost::shared_ptr<stream::filters::FpsFilter<lin_algebra::vec3f> > fpsed_gyro(
			new stream::filters::FpsFilter<lin_algebra::vec3f>(platform.gyro_sensor)
	);
	platform.gyro_sensor = fpsed_gyro;

	// create the cockpit and run it
	autopilot::Cockpit cockpit(platform);
	boost::shared_ptr<stream::filters::WatchFilter<lin_algebra::vec3f> > orientation(cockpit.orientation());
	boost::thread update_thread(stream_popper, oreintation);


	// export all the data
	std::cout << "Exporting all data" << std::endl;
	boost::shared_ptr<stream::TcpipClient> client = boost::make_shared<stream::TcpipClient>(argv[1], 0x6060);
	stream::StreamConnection conn(client);
	conn.export_pop_stream<lin_algebra::vec3f>(cockpit.watch_gyro_orientation(), "gyro_watch_orientation");
	conn.export_pop_stream<lin_algebra::vec3f>(cockpit.watch_rest_orientation(), "watch_rest_orientation");
	conn.export_pop_stream<lin_algebra::vec3f>(acc_watch->get_watch_stream(), "watch_acc_sensor");
	conn.export_pop_stream<lin_algebra::vec3f>(compass_watch->get_watch_stream(), "watch_compass_sensor");
	conn.export_pop_stream<lin_algebra::vec3f>(orientation->get_watch_stream(), "orientation");
	conn.export_pop_stream<float>(cockpit.watch_rest_reliability(), "reliability");
	conn.export_pop_stream<lin_algebra::vec2f>(cockpit.position(), "position");
	conn.export_pop_stream<float>(fpsed_gyro->get_fps_stream(), "gyro_fps");

	// run connection and make it block
	conn.run(false);
	return 0;
}
