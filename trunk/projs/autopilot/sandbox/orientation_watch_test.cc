#include "stream_watch.h"
#include "euler_angles_integral.h"
#include "static_filter.h"
#include "hw/itg3200_gyro.h"

typedef Watch<float,3> VecWatch;
typedef typename Watch<float,3>::vector_t vector_t;

vector_t calibrate_(VecGenerator<float,3>* gen) {
	vector_t sum;
	sum[0]= 0.; sum[1]=0.; sum[2]=0.;
	size_t num =0;
	while (true) {
		sum += gen->get_data();
		num++;
		std::cout << sum / float(num)  << std::endl;
	}
		return sum;
}



int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " <server_ip> " << std::endl;	
		exit(1);
	}
	typename StaticFilter<float, 3>::vector_t a, b;
	a[0] = -1.217; a[1] = -3.966; a[2] = 2.614;
	b[0] = 1.; b[1] = 1.; b[2] = 1.;

	Itg3200Gyro gen(2);
	StaticFilter<float,3> s(&gen, a, b);
	EulerAnglesIntegral<float> integ(&s);
	VecWatch watch(&integ, std::string(argv[1]),
			std::string("gyro_test"), -360.0f, 360.0f);

	watch.run();

	while (true) {
		VecWatch::vector_t v =  watch.get_data();
		//usleep(100);
	}
	return 0;
}
