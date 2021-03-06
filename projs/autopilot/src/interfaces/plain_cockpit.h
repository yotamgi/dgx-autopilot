#ifndef PLAIN_COCKPIT_H
#define PLAIN_COCKPIT_H

#include <stream/data_pop_stream.h>
#include <stream/filters/watch_filter.h>
#include <stream/util/lin_algebra.h>

namespace autopilot {

typedef stream::DataPopStream<lin_algebra::vec3f> 			vec3_stream;
typedef boost::shared_ptr<vec3_stream> 						vec3_stream_ptr;
typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec3_watch_stream;
typedef stream::DataPopStream<lin_algebra::vec2f> 			vec2_stream;
typedef boost::shared_ptr<vec2_stream> 						vec2_stream_ptr;
typedef stream::filters::WatchFilter<lin_algebra::vec2f> 	vec2_watch_stream;
typedef stream::DataPopStream<float> 						float_stream;
typedef boost::shared_ptr<float_stream>						float_stream_ptr;
typedef stream::filters::WatchFilter<float>					float_watch_stream;
typedef boost::shared_ptr<stream::DataPushStream<float> >   servo_stream_ptr;

/**
 * Normal Plain Cockpit.
 * Normal is every plain, which has orientation, speed and can be controlled
 * with 4 servo channels - yaw, pitch, roll and gas.
 */
class NormalPlainCockpit {
public:

	~NormalPlainCockpit() {}

	/**
	 * A method for updating the cockpit.
	 * If this function wont be called peridically, the cockpit data will not
	 * be updated.
	 * It is very important to run this method as many times as possible, in
	 * order to get the orientation as accurate as possible.
	 */
	virtual void update() = 0;

	virtual boost::shared_ptr<vec3_stream> orientation() = 0;

	virtual boost::shared_ptr<float_stream> ground_speed() = 0;

	virtual boost::shared_ptr<float_stream> air_speed() = 0;

	virtual boost::shared_ptr<vec2_stream> position() = 0;

	virtual boost::shared_ptr<float_stream> alt() = 0;

	virtual servo_stream_ptr tilt_servo() = 0;

	virtual servo_stream_ptr yaw_servo() = 0;

	virtual servo_stream_ptr pitch_servo() = 0;

	virtual servo_stream_ptr gas_servo() = 0;

	virtual boost::shared_ptr<float_stream> battery_state() = 0;

	virtual void alive() = 0;

};

} // namepsace autopilot

#endif // PLAIN_COCKPIT_H
