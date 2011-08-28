#include "util/watch.h"
#include "common/types.h"
#include "components/data_filters.h"
#include "hw/itg3200_gyro.h"

int main(int argc, char** argv) {

	Itg3200Gyro gen(2);
	IntergralFilter integ(&gen);
	VecWatch watch(&integ, "192.168.0.19");

	watch.run();

	while (true) {
		watch.get_data();
		usleep(10000);
	}
	return 0;
}
