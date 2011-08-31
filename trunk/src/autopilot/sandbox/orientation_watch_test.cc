#include "util/watch.h"
#include "integral_filter.h"
#include "hw/itg3200_gyro.h"

typedef Watch<float,3> VecWatch;

int main(int argc, char** argv) {

	Itg3200Gyro gen(2);
	IntergralFilter<float,3> integ(&gen, -360.0, 360.0f);
	VecWatch watch(&integ, std::string("192.168.0.19"),
			std::string("gyro_test"), -360.0f, 360.0f);

	watch.run();

	while (true) {
		watch.get_data();
		usleep(10000);
	}
	return 0;
}
