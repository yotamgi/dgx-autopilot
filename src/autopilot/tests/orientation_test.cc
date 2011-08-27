#include "util/watch.h"
#include "common/types.h"
#include "hw/itg3200_gyro.h"
#include <iostream>

int main(int argc, char** argv) {

	Itg3200Gyro gen(2);


	while (true) {
		boost::shared_ptr<angular_velocity_t> av = gen.get_data();
		std::cout << std::hex << "(" << av->x << ", " << av->y << ", " << av->z << ")" << std::endl;
		usleep(100000);
	}
	return 0;
}
