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
	if (argc > 2) {
		for (size_t i=1; i<(size_t)argc; i++) {

			if (std::string(argv[i]) == "--sensor-sim") {
				if ((size_t)argc < i+1) {
					std::cout << "2" << std::endl;
					usage(argv[0]);
					exit(1);
				} else {
					sim_addr = argv[2];
					i++;
				}
				sim = true;
				continue;
			}
			else if (std::string(argv[i]) == "--present-local") {
				present_local = true;
				continue;
			}
			else {
				std::cout << "1" << std::endl;
				usage(argv[0]);
				exit(1);
			}

		}
	} else if (argc != 1) {
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
		presenter.addAngleStream(cockpit.orientation_gyro(), irr::core::vector3df(-10., 0., 0.));
		presenter.addAngleStream(cockpit.orientation_rest(), irr::core::vector3df(10., 0., 0.));
		presenter.addVecStream(platform->acc_sensor(), irr::core::vector3df(10., 0., 0.));
		presenter.addVecStream(platform->compass_sensor(), irr::core::vector3df(10., 0., 0.));
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
