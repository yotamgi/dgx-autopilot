#include "stream/stream_exporter.h"
#include <stream/filters/rotation_integral.h>
#include <stream/filters/static_filter.h>
#include <stream/filters/matrix_to_euler_filter.h>
#include <stream/util/time.h>
#include "hw/itg3200_gyro.h"
#include "hw/adxl345_acc.h"
#include "hw/hmc5843_compass.h"

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

	stream::StreamExporter exporter;

	// gyro
	typename stream::filters::StaticFilter<float, 3>::vector_t a, b;
	a[0] = -1.217; a[1] = -3.966; a[2] = 2.614;
	b[0] = 1.; b[1] = 1.; b[2] = 1.;

	Itg3200Gyro gyro(2);
	stream::filters::StaticFilter<float,3> s(&gyro, a, b);
	stream::filters::RotationIntegral gyro_rot(&s);
	stream::filters::MatrixToEulerFilter gyro_rot_euler(&gyro_rot);

	exporter.register_stream(&gyro_rot_euler, std::string("gyro_test"));

	// accelerometer
	Adxl345Acc acc(2);
	exporter.register_stream(&acc, std::string("acc_test"));

	// compass
	Hmc5843Compass c(2);
	exporter.register_stream(&c, std::string("compass_test"));

	std::cout << "Watiging for connections. " << std::endl;
	exporter.run();

	return 0;
}
