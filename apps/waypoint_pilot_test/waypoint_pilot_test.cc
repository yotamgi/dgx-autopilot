#include <autopilot/platform/dgx1_platform.h>
#include <autopilot/platform/platform_export_import.h>
#include <autopilot/cockpit.h>
#include <autopilot/waypoint_pilot.h>
#include <stream/util/tcpip_connection.h>
#include <gs/map_stream_view.h>
#include <simulator/simulator_platform.h>
#include <simulator/platforms.h>

#include <boost/thread.hpp>

#include <iostream>
#include <string>
#include <cstdlib>


void usage_and_exit(std::string argv0) {
	std::cerr << "usage: " << argv0 << " [--platform hw|sim|ip_addr]" << std::endl;
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

void create_gs(boost::shared_ptr<autopilot::Cockpit> cockpit, int argc, char** argv) {
	// Show a map
	QApplication app(argc, argv);
	QWidget* window = new QWidget();
	QHBoxLayout* layout = new QHBoxLayout();
	gs::MapStreamView* map_view = new gs::MapStreamView(cockpit->position(),
			0.2, QSize(400, 300), std::string("../../projs/ground_station/data/map"));
	layout->addWidget(map_view);
	window->setLayout(layout);
	window->show();
	app.exec();
}

int main(int argc, char** argv) {

	// parse some args
	std::string platform_type = "sim"; // default is hw
	if (argc == 3) {
		if (std::string(argv[1]) != "--platform") usage_and_exit(argv[0]);
		platform_type = argv[2];
	} else if (argc == 1) {
	} else {
		usage_and_exit(argv[0]);
	}

	// configure the pilot
	autopilot::WaypointPilot::Params pilot_params;
	pilot_params.max_climbing_angle = 15.;
	pilot_params.max_decending_angle = -10.;
	pilot_params.climbing_gas = 100.;
	pilot_params.decending_gas = 20.;
	pilot_params.avg_gas = 40;
	pilot_params.avg_pitch_angle = 0.;
	pilot_params.max_roll_angle = 40.;
	pilot_params.pitch_severity = 20.;
	pilot_params.roll_severity = 20.;

	// create the platform according to the args
	autopilot::NormalPlainPlatform platform;
	if (platform_type == "hw") {
	    platform = autopilot::create_dgx1_platform();
	} else if (platform_type == "sim") {
	    platform = *simulator::create_simulator_platform(simulator::platforms::dgx_platform);
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
	boost::thread(create_gs, cockpit, argc, argv);

	// Show a map
	//create_gs(cockpit);
	while (true);
	return 0;
}
