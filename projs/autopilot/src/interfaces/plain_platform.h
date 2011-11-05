#ifndef PLAIN_PLATFORM_H_
#define PLAIN_PLATFORM_H_

#include <stream/generators.h>
#include <boost/shared_ptr.hpp>
#include <stream/util/lin_algebra.h>
#include "interfaces/servo.h"

namespace autopilot {

/**
 * Normal Plain Platform.
 * Represents a normal plain. The requirements are
 *  - To have Gyro, Acc and Compass Data Sensors
 *  - To have GPS sensor
 *  - To be controlled by Servos.
 */
class NormalPlainPlatform {
public:
	typedef boost::shared_ptr<stream::DataGenerator<lin_algebra::vector_t> > vec_stream_ptr;

	virtual ~NormalPlainPlatform() {}

	virtual vec_stream_ptr acc_sensor() = 0;

	virtual vec_stream_ptr gyro_sensor() = 0;

	virtual vec_stream_ptr compass_sensor() = 0;

	virtual vec_stream_ptr gps_sensor() = 0;

	virtual Servo* tilt_servo() = 0;

	virtual Servo* yaw_servo() = 0;

	virtual Servo* pitch_servo() = 0;

	virtual Servo* gas_servo() = 0;
};

} //namespace autopilot


#endif // PLAIN_PLATFORM_H_
