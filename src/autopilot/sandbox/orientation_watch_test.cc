#include "util/watch.h"
#include "common/types.h"
#include "components/data_filters.h"
#include "hw/itg3200_gyro.h"

typedef Watch<vector_t, 3> VecWatch;

int main(int argc, char** argv) {

	Itg3200Gyro gen(2);
	IntergralFilter integ(&gen, -360.0, 360.0f);
	VecWatch watch(&integ, "192.168.0.19", "gyro_test", -360.0f, 360.0f);

	watch.run();

	while (true) {
		watch.get_data();
		usleep(10000);
	}
	return 0;
}
