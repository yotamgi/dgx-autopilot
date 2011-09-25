#include "stream/stream_exporter.h"
#include <stream/filters/euler_angles_integral.h>
#include <stream/filters/static_filter.h>
#include <stream/util/time.h>
#include "hw/itg3200_gyro.h"

typedef typename stream::VecGenerator<float,3>::vector_t vector_t;

vector_t calibrate_(stream::VecGenerator<float,3>* gen) {
	vector_t sum;
	sum[0]= 0.; sum[1]=0.; sum[2]=0.;
	size_t num =0;
	while (true) {
		typename stream::VecGenerator<float,3>::vector_t a = gen->get_data();
		for (size_t i=0; i<3; i++) {
			sum[i] += a[i];
		}
		num++;
		for (size_t i=0; i<3; i++) {
			std::cout << sum[i] / float(num)  << ", " << std::endl;
		}
	}
		return sum;
}



int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " <server_ip> " << std::endl;	
		exit(1);
	}
	typename stream::filters::StaticFilter<float, 3>::vector_t a, b;
	a[0] = -1.217; a[1] = -3.966; a[2] = 2.614;
	b[0] = 1.; b[1] = 1.; b[2] = 1.;

	Itg3200Gyro gen(2);
	stream::filters::StaticFilter<float,3> s(&gen, a, b);
	stream::filters::EulerAnglesIntegral<float> integ(&s);

	stream::StreamExporter exporter;
	exporter.register_stream(&integ, std::string("gyro_test"));

	exporter.run();

	return 0;
}
