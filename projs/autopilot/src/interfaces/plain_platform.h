#ifndef PLAIN_PLATFORM_H_
#define PLAIN_PLATFORM_H_

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/filters/watch_filter.h>
#include <boost/shared_ptr.hpp>
#include <stream/util/lin_algebra.h>

namespace autopilot {

typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec3_watch_stream;
typedef boost::shared_ptr<stream::DataPushStream<lin_algebra::vec3f> > gps_pos_reciever_ptr;
typedef boost::shared_ptr<stream::DataPushStream<float> > gps_speed_dir_reciever_ptr;
typedef boost::shared_ptr<stream::DataPushStream<float> > gps_speed_mag_reciever_ptr;

/**
 * Normal Plain Platform.
 * Represents a normal plain. The requirements are
 *  - To have Gyro, Acc and Compass Data Sensors
 *  - To have GPS sensor
 *  - To be controlled by Servos.
 */
struct NormalPlainPlatform {

	boost::shared_ptr<vec3_watch_stream> acc_sensor;

	boost::shared_ptr<vec3_watch_stream> gyro_sensor;

	boost::shared_ptr<vec3_watch_stream> compass_sensor;

	boost::shared_ptr<stream::PushGenerator<lin_algebra::vec3f> > gps_pos_generator;

	boost::shared_ptr<stream::PushGenerator<float> > gps_speed_dir_generator;

	boost::shared_ptr<stream::PushGenerator<float> > gps_speed_mag_generator;

	boost::shared_ptr<stream::DataPushStream<float> > tilt_servo;

	boost::shared_ptr<stream::DataPushStream<float> > yaw_servo;

	boost::shared_ptr<stream::DataPushStream<float> > pitch_servo;

	boost::shared_ptr<stream::DataPushStream<float> > gas_servo;
};

} //namespace autopilot


#endif // PLAIN_PLATFORM_H_
