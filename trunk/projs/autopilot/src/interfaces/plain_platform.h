#ifndef PLAIN_PLATFORM_H_
#define PLAIN_PLATFORM_H_

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/filters/watch_filter.h>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <stream/util/lin_algebra.h>

namespace autopilot {

typedef stream::DataPopStream<lin_algebra::vec3f> 	vec3_pop_stream;
typedef stream::DataPopStream<float> 				float_pop_stream;
typedef stream::DataPushStream<float> 				float_push_stream;
typedef stream::PushGenerator<lin_algebra::vec3f> 	vec3_push_gen;
typedef stream::PushGenerator<float> 				float_push_gen;

/**
 * Normal Plain Platform.
 * Represents a normal plain. The requirements are
 *  - To have Gyro, Acc and Compass Data Sensors
 *  - To have GPS sensor
 *  - To be controlled by Servos.
 */
struct NormalPlainPlatform {

	// The ordinary pop sensors
	boost::shared_ptr<vec3_pop_stream> acc_sensor;
	boost::shared_ptr<vec3_pop_stream> gyro_sensor;
	boost::shared_ptr<vec3_pop_stream> compass_sensor;
	boost::shared_ptr<float_pop_stream> airspeed_sensor;
	boost::shared_ptr<float_pop_stream> alt_sensor;
	boost::shared_ptr<float_pop_stream> battery_sensor;

	// The Gps stream generators
	boost::shared_ptr<vec3_push_gen>  gps_pos_generator;
	boost::shared_ptr<float_push_gen> gps_speed_dir_generator;
	boost::shared_ptr<float_push_gen> gps_speed_mag_generator;

	// The Servos
	boost::shared_ptr<float_push_stream> tilt_servo;
	boost::shared_ptr<float_push_stream> yaw_servo;
	boost::shared_ptr<float_push_stream> pitch_servo;
	boost::shared_ptr<float_push_stream> gas_servo;

	boost::function<void(void)> alive;
};

} //namespace autopilot


#endif // PLAIN_PLATFORM_H_
