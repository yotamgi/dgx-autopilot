#include <stream/stream_exporter.h>
#include <platform/dgx1_platform.h>
#include <platform/dgx1_simulator_platform.h>
#include <cockpit.h>
#include <stream_watch/stream_presenter_3d.h>

#include <string>
#include <iostream>

void usage(std::string arg0) {
	std::cout << arg0 << " [--sensor-sim <address> ]  [--present-local]  [--help]" << std::endl;
}

int main(int argc, char** argv) {
	bool sim = false;
	std::string sim_addr;
	bool present_local = false;
	if (argc > 1) {
		if (std::string(argv[1]) == "--sensor-sim") {
			if (argc != 2) {
				usage(argv[0]);
				exit(1);
			} else {
				sim_addr = argv[2];
			}
			sim = true;
		} else if (std::string(argv[1]) == "--present-local") {
			present_local = true;
		} else {
			usage(argv[0]);
			exit(1);
		}
	} else if (argc != 0) {
		usage(argv[0]);
		exit(1);
	}


	boost::shared_ptr<autopilot::NormalPlainPlatform> platform(
		sim?
			(autopilot::NormalPlainPlatform*)new autopilot::DGX1SimulatorPlatform(sim_addr) :
			(autopilot::NormalPlainPlatform*)new autopilot::DGX1Platform()
	);

	autopilot::Cockpit cockpit(platform);

	if (present_local) {
		StreamPresenter presenter;
		char c;
		std::cout << "gyro(g) or rest(r)? ";
		std::cin >> &c;
		if (c == 'g') {
			presenter.setAngleStream(cockpit.orientation_gyro());
		} else if (c == 'r') {
			presenter.setAngleStream(cockpit.orientation_rest());
		}
		std::cout << std::endl;
		presenter.run(false);
	} else {
		stream::StreamExporter exp;
		exp.register_stream(cockpit.orientation_gyro(), "cockpit_gyro");
		exp.register_stream(cockpit.orientation_rest(), "cockpit_rest");
		exp.register_stream(cockpit.orientation(), "cockpit_orientation");
		exp.run();
	}

	while (true);

	return 0;

}
