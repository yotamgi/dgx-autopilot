#include "hw/itg3200_gyro.h"
#include <iostream>
#include "integral_filter.h"

int main(int argc, char** argv) {

	Itg3200Gyro gyro(2);
	IntergralFilter<float,3> gen(&gyro, 10000.0f, -10000.0f);

	while (true) {
		Itg3200Gyro::vector_t av = gen.get_data();
		std::cout << std::hex << "(" << av[0] << ", " << av[1] << ", " << av[2]  << ")" << std::endl;
		usleep(100000);
	}
	return 0;
}
