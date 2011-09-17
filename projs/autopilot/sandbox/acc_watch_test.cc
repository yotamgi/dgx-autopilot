#include "stream_watch.h"
#include "euler_angles_integral.h"
#include "static_filter.h"
#include <util/time.h>
#include "hw/adxl345_acc.h"

typedef Watch<float,3> VecWatch;
typedef typename Watch<float,3>::vector_t vector_t;

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " <server_ip> " << std::endl;	
		exit(1);
	}
	Adxl345Acc gen(2);
	VecWatch watch(&gen, std::string(argv[1]),
			std::string("acc_test"), -360.0f, 360.0f);

	watch.run();

	size_t times;
	Timer t;
	while (true) {
		VecWatch::vector_t v =  watch.get_data();
		times ++;
		if (t.passed() > 1.0) {
			t.reset();
			std::cout << "FPS " << times << std::endl;
			times = 0;
		}
	}
	return 0;
}
