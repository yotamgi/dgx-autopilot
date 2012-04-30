#include <autopilot/platform/dgx1_platform.h>
#include <autopilot/cockpit.h>
#include <autopilot/waypoint_pilot.h>
#include <stream/util/tcpip_connection.h>

#include <boost/thread.hpp>

#include <iostream>
#include <string>
#include <cstdlib>


void usage_and_exit(std::string argv0) {
	std::cerr << "usage: " << argv0 << " [--sensor-sim <address>]" << std::endl;
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
	std::string sim_address;
	if (argc == 3) {
		if (std::string(argv[1]) != "--sensor-sim") usage_and_exit(argv[0]);
		sim_address = argv[2];
	} else if (argc == 1) {
	} else {
		usage_and_exit(argv[0]);
	}

	// configure the pilot
	autopilot::WaypointPilotParams pilot_params;
	pilot_params.max_climbing_angle = 15.;
	pilot_params.climbing_gas = 100.;
	pilot_params.max_decending_angle = -10.;
	pilot_params.decending_gas = 20.;
	pilot_params.avg_gas = 40;
	pilot_params.avg_pitch = 0.;
	pilot_params.max_tilt_angle = 20.;

	// create the platform according to the args
	autopilot::NormalPlainPlatform platform(
		(sim_address == "") ?
			autopilot::create_dgx1_platform() :
			autopilot::create_dgx1_simulator_platform(boost::make_shared<stream::TcpipServer>(sim_address, 0x6060))
	);

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
