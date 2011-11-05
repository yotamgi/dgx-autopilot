#ifndef DGX1_PLATFORM_H_
#define DGX1_PLATFORM_H_

#include "interfaces/plain_platform.h"
#include <stdexcept>

namespace autopilot {

typedef boost::shared_ptr<stream::DataGenerator<lin_algebra::vector_t> > vec_stream_ptr;

class DGX1Platform : public NormalPlainPlatform {
public:
	DGX1Platform();
	~DGX1Platform(){}

	typedef boost::shared_ptr<stream::DataGenerator<lin_algebra::vector_t> > vec_stream_ptr;

	vec_stream_ptr acc_sensor();

	vec_stream_ptr gyro_sensor();

	vec_stream_ptr compass_sensor();

	vec_stream_ptr gps_sensor();

	Servo* tilt_servo();

	Servo* yaw_servo();

	Servo* pitch_servo();

	Servo* gas_servo();

private:
	vec_stream_ptr m_acc;
	vec_stream_ptr m_gyro;
	vec_stream_ptr m_compass;
};

} //namespace autopilot

#endif /* DGX1_PLATFORM_H_ */
