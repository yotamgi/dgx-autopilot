#include <stream/util/tcpip_connection.h>
#include <stream/stream_connection.h>
#include <stream/filters/gyro_to_av_matrix.h>
#include <stream/filters/static_filter.h>
#include <stream/filters/matrix_to_euler_filter.h>
#include <stream/util/time.h>
#include "platform/hw/itg3200_gyro.h"
#include "platform/hw/adxl345_acc.h"
#include "platform/hw/hmc5843_compass.h"

#include <boost/make_shared.hpp>

typedef typename lin_algebra::vec3f vector_t;

vector_t calibrate_(stream::DataPopStream<lin_algebra::vec3f>* gen) {
	vector_t sum;
	sum[0]= 0.; sum[1]=0.; sum[2]=0.;
	size_t num =0;
	while (true) {
		typename lin_algebra::vec3f a = gen->get_data();
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

	boost::shared_ptr<stream::TcpipClient> client = boost::make_shared<stream::TcpipClient>(argv[1], 0x6060);
	stream::StreamConnection conn(client);

	// gyro
	lin_algebra::vec3f a;
	a[0] = -1.217; a[1] = -3.966; a[2] = 2.614;

	using namespace stream;
	using namespace stream::filters;
	using namespace boost;

	shared_ptr<autopilot::Itg3200Gyro> gyro = make_shared<autopilot::Itg3200Gyro>(2);
	shared_ptr<StaticFilter<lin_algebra::vec3f> > s = make_shared<StaticFilter<lin_algebra::vec3f> >(gyro, a);
	shared_ptr<GyroToAVMatrix> gyro_rot = make_shared<GyroToAVMatrix>(s);
	shared_ptr<MatrixToEulerFilter> gyro_rot_euler = make_shared<MatrixToEulerFilter>(gyro_rot);

	conn.export_pop_stream<lin_algebra::vec3f>(gyro_rot_euler, std::string("gyro_test"));

	// accelerometer
	shared_ptr<autopilot::Adxl345Acc> acc(new autopilot::Adxl345Acc(2));
	conn.export_pop_stream<lin_algebra::vec3f>(acc, std::string("acc_test"));

	// compass
	shared_ptr<autopilot::Hmc5843Compass> c(new autopilot::Hmc5843Compass(2));
	conn.export_pop_stream<lin_algebra::vec3f>(c, std::string("compass_test"));

	std::cout << "Waiting for connections. " << std::endl;
	conn.run(false);

	return 0;
}
