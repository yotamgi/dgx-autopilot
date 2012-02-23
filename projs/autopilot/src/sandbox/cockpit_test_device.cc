#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <platform/dgx1_platform.h>
#include <cockpit.h>

#include <string>
#include <iostream>

typedef stream::filters::WatchFilter<lin_algebra::vec3f> vec3_watch_stream;

void update_cockpit(autopilot::Cockpit* cockpit) {
	while (true) {
		cockpit->orientation()->get_data();
	}
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Please enter the address to export to" << std::endl;
		exit(1);
	}

	autopilot::NormalPlainPlatform platform = autopilot::create_dgx1_platform();

	// add a watch stream on the compass and acc stream
	boost::shared_ptr<vec3_watch_stream> compass_watch(new vec3_watch_stream(platform.compass_sensor));
	platform.compass_sensor = compass_watch;
	boost::shared_ptr<vec3_watch_stream> acc_watch(new vec3_watch_stream(platform.acc_sensor));
	platform.acc_sensor = acc_watch;


	autopilot::Cockpit cockpit(platform);

	boost::thread update_thread(update_cockpit, &cockpit);

	std::cout << "Exporting all data" << std::endl;

	boost::shared_ptr<stream::TcpipClient> client = boost::make_shared<stream::TcpipClient>(argv[1], 0x6060);
	stream::StreamConnection conn(client);
	conn.export_pop_stream<lin_algebra::vec3f>(cockpit.watch_gyro_orientation(), "gyro_watch_orientation");
	conn.export_pop_stream<lin_algebra::vec3f>(cockpit.watch_rest_orientation(), "watch_rest_orientation");
	conn.export_pop_stream<lin_algebra::vec3f>(acc_watch->get_watch_stream(), "watch_acc_sensor");
	conn.export_pop_stream<lin_algebra::vec3f>(compass_watch->get_watch_stream(), "watch_compass_sensor");
	conn.export_pop_stream<lin_algebra::vec3f>(cockpit.orientation(), "orientation");
	conn.export_pop_stream<float>(cockpit.watch_rest_reliability(), "reliability");
	conn.export_pop_stream<lin_algebra::vec2f>(cockpit.position(), "position");

	// run connection and make it block
	conn.run(false);
	return 0;
}
