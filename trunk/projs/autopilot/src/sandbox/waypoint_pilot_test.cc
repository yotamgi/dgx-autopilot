#include <platform/dgx1_platform.h>
#include <cockpit.h>
#include <waypoint_pilot.h>
#include <stream/util/tcpip_connection.h>
#include <gs/map_stream_view.h>

#include <boost/thread.hpp>
#include <boost/assign/std/vector.hpp>

#include <iostream>
#include <string>

using namespace boost::assign;


void usage_and_exit(std::string argv0) {
	std::cerr << "usage: " << argv0 << " [--sensor-sim <address>]" << std::endl;
	exit(1);
}

void create_gs(boost::shared_ptr<autopilot::Cockpit> cockpit, int argc, char** argv) {
	// Show a map
	QApplication app(argc, argv);
	QWidget* window = new QWidget();
	QHBoxLayout* layout = new QHBoxLayout();
	gs::MapStreamView* map_view = new gs::MapStreamView(cockpit->position(),
			0.2, QSize(400, 300), std::string("../ground_station/data/map"));
	layout->addWidget(map_view);
	window->setLayout(layout);
	window->show();
	app.exec();
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

	autopilot::WaypointPilot::waypoint_list path;
	path += autopilot::WaypointPilot::waypoint(lin_algebra::create_vec2f(300., 300.), 100.),
			autopilot::WaypointPilot::waypoint(lin_algebra::create_vec2f(300., -300.), 100.),
			autopilot::WaypointPilot::waypoint(lin_algebra::create_vec2f(-300., -300.), 100.),
			autopilot::WaypointPilot::waypoint(lin_algebra::create_vec2f(-300., 300.), 100.);

	pilot.set_path(path);

	pilot.start(true);
	boost::thread(create_gs, cockpit, argc, argv);

	// Show a map
	//create_gs(cockpit);
	while (true);
	return 0;
}
