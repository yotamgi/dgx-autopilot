#include <stream/stream_exporter.h>
#include <platform/dgx1_platform.h>
#include <platform/dgx1_simulator_platform.h>
#include <cockpit.h>
#include <stream_watch/stream_presenter_3d.h>

#include <string>
#include <iostream>

void usage(std::string arg0) {
	std::cout << arg0 << " [--sensor-sim <address> ]  [--present-from <address> ] [--present-local]  [--help]" << std::endl;
}

int main(int argc, char** argv) {
	bool sim = false;
	std::string sim_addr;
	std::string present_addr;
	bool present_local = false;
	if (argc > 2) {
		for (size_t i=1; i<(size_t)argc; i++) {

			if (std::string(argv[i]) == "--sensor-sim") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(1);
				} else {
					sim_addr = argv[i+1];
					i++;
				}
				sim = true;
				continue;
			}
			else if (std::string(argv[i]) == "--present-local") {
				present_local = true;
				continue;
			}
			else if (std::string(argv[i]) == "--present-from") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(2);
				} else {
					present_addr = argv[i+1];
					i++;
				}
				continue;
			}
			else {
				usage(argv[0]);
				exit(1);
			}

		}
	} else if (argc != 1) {
		usage(argv[0]);
		exit(1);
	}

	StreamPresenter presenter;

	if (present_addr != "")  {
		std::cout << "Presenting from addr " << present_addr << std::endl;
		stream::StreamImporter imp(present_addr);

		typedef stream::DataGenerator<lin_algebra::vec3f> vs;
		typedef stream::DataGenerator<float> fs;

		// the left one
		presenter.addAngleStream(imp.import_stream<vs>("gyro_watch_orientation"), irr::core::vector3df(-20., 0., 0.));

		// the mid one
		presenter.addAngleStream(imp.import_stream<vs>("watch_rest_orientation"), irr::core::vector3df(0., 0., 0.));
		presenter.addVecStream(imp.import_stream<vs>("watch_acc_sensor"), irr::core::vector3df(0., 0., 0.));
		presenter.addVecStream(imp.import_stream<vs>("watch_compass_sensor"), irr::core::vector3df(0., 0., 0.));

		// the right one
		presenter.addAngleStream(imp.import_stream<vs>("orientation"), irr::core::vector3df(20., 0., 0.));

		// the reliable stream
		presenter.addSizeStream(imp.import_stream<fs>("reliability"));

		presenter.run(false);

	} else {


		boost::shared_ptr<autopilot::NormalPlainPlatform> platform(
			sim?
				(autopilot::NormalPlainPlatform*)new autopilot::DGX1SimulatorPlatform(sim_addr) :
				(autopilot::NormalPlainPlatform*)new autopilot::DGX1Platform()
		);

		autopilot::Cockpit cockpit(platform);

		if (present_local) {
			std::cout << "Presenting locally" << std::endl;
			// the left one
			presenter.addAngleStream(cockpit.watch_gyro_orientation(), irr::core::vector3df(-20., 0., 0.));

			// the mid one
			presenter.addAngleStream(cockpit.watch_rest_orientation(), irr::core::vector3df(0., 0., 0.));
			presenter.addVecStream(platform->acc_sensor()->get_watch_stream(), irr::core::vector3df(0., 0., 0.));
			presenter.addVecStream(platform->compass_sensor()->get_watch_stream(), irr::core::vector3df(0., 0., 0.));

			// the right one
			presenter.addAngleStream(cockpit.orientation()->get_watch_stream(), irr::core::vector3df(20., 0., 0.));

			// the reliable stream
			presenter.addSizeStream(cockpit.watch_rest_reliability());

			presenter.run(true);
		}  else {
			std::cout << "Exporting all data" << std::endl;
			stream::StreamExporter exp;
			exp.register_stream(cockpit.watch_gyro_orientation(), "gyro_watch_orientation");
			exp.register_stream(cockpit.watch_rest_orientation(), "watch_rest_orientation");
			exp.register_stream(platform->acc_sensor()->get_watch_stream(), "watch_acc_sensor");
			exp.register_stream(platform->compass_sensor()->get_watch_stream(), "watch_compass_sensor");
			exp.register_stream(cockpit.orientation(), "orientation");
			exp.register_stream(cockpit.watch_rest_reliability(), "reliability");
			exp.run();
		}

		// to apply the watches
		while (true) {
			cockpit.orientation()->get_data();
		}
	}
	return 0;

}
