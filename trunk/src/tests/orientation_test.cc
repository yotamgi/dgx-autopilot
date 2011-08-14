#include "util/watch.h"
#include "common/types.h"
#include "hw/itg3200_gyro.h"

int main(int argc, char** argv) {

	Itg3200Gyro gen;
	VecWatch watch(&gen, "localhost");

	watch.run();

	while (true) {
		watch.get_data();
		usleep(10000);
	}
	return 0;
}
