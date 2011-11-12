#ifndef PLAIN_PLATFORM_H_
#define PLAIN_PLATFORM_H_

#include <stream/generators.h>
#include <stream/filters/watch_filter.h>
#include <boost/shared_ptr.hpp>
#include <stream/util/lin_algebra.h>
#include "interfaces/servo.h"

namespace autopilot {

typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec_watch_stream;

/**
 * Normal Plain Platform.
 * Represents a normal plain. The requirements are
 *  - To have Gyro, Acc and Compass Data Sensors
 *  - To have GPS sensor
 *  - To be controlled by Servos.
 */
class NormalPlainPlatform {
public:
	typedef boost::shared_ptr<stream::DataGenerator<lin_algebra::vec3f> > vec_stream_ptr;

	virtual ~NormalPlainPlatform() {}

	virtual boost::shared_ptr<vec_watch_stream> acc_sensor() = 0;

	virtual boost::shared_ptr<vec_watch_stream> gyro_sensor() = 0;

	virtual boost::shared_ptr<vec_watch_stream> compass_sensor() = 0;

	virtual boost::shared_ptr<vec_watch_stream> gps_sensor() = 0;

	virtual Servo* tilt_servo() = 0;

	virtual Servo* yaw_servo() = 0;

	virtual Servo* pitch_servo() = 0;

	virtual Servo* gas_servo() = 0;
};

} //namespace autopilot


#endif // PLAIN_PLATFORM_H_
