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

	if (stress) {
		std::cout << "Running stress test..." << std::endl;
		float heading = 1.0;
		float target = 0;
		for(int i=0; ; i++) {
			target += heading*0.6;

			if (target > 100.)
				heading = -1.;
			else if (target < 0.)
				heading = 1.;
			
			m.getServoChannel(0)->set_data(target); 
			usleep(5000);
			m.getServoChannel(1)->set_data(target); 
			usleep(5000);
			m.getServoChannel(2)->set_data(target); 
			usleep(5000);
			m.getServoChannel(3)->set_data(target); 
			usleep(5000);
			m.getServoChannel(4)->set_data(target); 
			usleep(5000);
			m.getServoChannel(5)->set_data(target); 
			usleep(5000);
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
