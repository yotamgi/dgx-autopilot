#include <autopilot/platform/dgx1_platform.h>
#include <autopilot/platform/platform_export_import.h>
#include <autopilot/cockpit.h>
#include <autopilot/waypoint_pilot.h>
#include <stream/util/tcpip_connection.h>

#include <boost/thread.hpp>

#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>


void usage_and_exit(std::string argv0) {
	std::cerr << "usage: " << argv0 << " [--platform hw|ip_addr]" << std::endl;
	exit(1);
}

float frand() {
	return float(std::rand())/float(RAND_MAX);
}

autopilot::WaypointPilot::waypoint_list create_rand_path(
		size_t size,
		lin_algebra::vec2f begin)
{
	autopilot::WaypointPilot::waypoint_list ans;

	lin_algebra::vec2f curr_point = begin;

	for (size_t i=0; i<size; i++) {

		// create the direction
		lin_algebra::vec2f direction;
		direction[0] = 1. - 2.*frand();
		direction[1] = 1. - 2.*frand();
		direction = lin_algebra::normalize(direction);

		autopilot::WaypointPilot::waypoint new_point;
		new_point.target = curr_point + (150. + 200*frand())*direction;
		new_point.altitude = 100. + 100.*(frand() - 0.5);

		curr_point = new_point.target;
		ans.push_back(new_point);
	}

	return ans;
}

int main(int argc, char** argv) {

	// parse some args
	std::string platform_type = "hw";
	if (argc == 3) {
		if (std::string(argv[1]) != "--platform") usage_and_exit(argv[0]);
		platform_type = argv[2];
	} else if (argc == 1) {
	} else {
		usage_and_exit(argv[0]);
	}

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

	// create the platform according to the args
	autopilot::NormalPlainPlatform platform;
	if (platform_type == "hw") {
	    platform = autopilot::create_dgx1_platform();
	} else if (platform_type == "sim") {
	    throw std::runtime_error("simulator platform is not supported on device");
	} else {
	    platform = *autopilot::import_platform(boost::make_shared<stream::TcpipServer>(platform_type , 0x6060));
	}

	boost::shared_ptr<autopilot::Cockpit> cockpit(boost::make_shared<autopilot::Cockpit>(platform));
	autopilot::WaypointPilot pilot(pilot_params, cockpit);

	autopilot::WaypointPilot::waypoint_list path =
			create_rand_path(
					1000,
					lin_algebra::create_vec2f(0., 0.)
	);

	pilot.set_path(path);

	pilot.start(true);
	while (true);
	return 0;
}
