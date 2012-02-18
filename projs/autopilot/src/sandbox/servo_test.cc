#include "platform/hw/maestro_servo_controller.h"
#include <stdlib.h>
#include <iostream>
#include <string>

void print_usage_and_exit() {
	std::cout << "USAGE: servo_test PORT. i.e. /dev/ttyACM1 [-stress]  \n" << std::endl;
	exit(-1); // EXIT
}

int main(int argc, char** argv) {
	if (argc != 2 && argc != 3) {
		print_usage_and_exit();
	}

	bool stress = false;
	if (argc == 3){
		if (std::string(argv[2]) != "-stress") {
			print_usage_and_exit();
		}
		stress = true;
	}

	autopilot::MaestroServoController m(argv[1]);
	int target;

	if (stress) {
		std::cout << "Running stress test..." << std::endl;
		while(1) {
			for(target=4000; target < 8001; target++){
				std::cout << "target is " <<  target << std::endl;
				for (int i=0; i<1000; i++ ) {

					try {
					m.getServoChannel(5)->set_data(float(target-4000)/40); // in percentage
					} catch (...) {
					}
				}
			}
		}
	} else {
		while (true) {
			for (size_t i=0; i<6; i++) {
				m.getServoChannel(i)->set_data(100.);
			}
			usleep(1000000);

			for (size_t i=0; i<6; i++) {
				m.getServoChannel(i)->set_data(0.);
			}
			usleep(1000000);
		}
	}
	return 0;
}
