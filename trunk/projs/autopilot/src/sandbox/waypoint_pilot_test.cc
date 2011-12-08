#include <platform/dgx1_platform.h>
#include <cockpit.h>
#include <waypoint_pilot.h>

#include <stream/util/tcpip_connection.h>

#include <iostream>
#include <string>

void usage_and_exit(std::string argv0) {
	std::cerr << "usage: " << argv0 << " [--sensor-sim <address>]" << std::endl;
	exit(1);
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

	// create the platform according to the args
	boost::shared_ptr<autopilot::NormalPlainPlatform> platform(
		(sim_address == "") ?
			(autopilot::NormalPlainPlatform*)new autopilot::DGX1Platform() :
			(autopilot::NormalPlainPlatform*)new autopilot::DGX1SimulatorPlatform(boost::make_shared<stream::TcpipServer>(sim_address, 0x6060))
	);

	boost::shared_ptr<autopilot::Cockpit> cockpit(boost::make_shared<autopilot::Cockpit>(platform));
	autopilot::WaypointPilot pilot(cockpit);

	lin_algebra::vec2f waypoint;
	waypoint.zeros();
	pilot.to_waypoint(waypoint, 60.);

	pilot.start(false);
}
