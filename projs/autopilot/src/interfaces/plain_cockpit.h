#ifndef PLAIN_COCKPIT_H
#define PLAIN_COCKPIT_H

#include <stream/data_pop_stream.h>
#include <stream/filters/watch_filter.h>
#include <stream/util/lin_algebra.h>

namespace autopilot {

typedef stream::DataPopStream<lin_algebra::vec3f> 			vec_stream;
typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec_watch_stream;
typedef stream::DataPopStream<float> 						float_stream;
typedef stream::filters::WatchFilter<float_stream>			float_watch_stream;
typedef boost::shared_ptr<stream::DataPushStream<float> >   servo_stream_ptr;

/**
 * Normal Plain Cockpit.
 * Normal is every plain, which has orientation, speed and can be controlled
 * with 4 servo channels - yaw, pitch, roll and gas.
 */
class NormalPlainCockpit {
public:

	~NormalPlainCockpit() {}

	virtual boost::shared_ptr<vec_watch_stream> orientation() = 0;

	virtual boost::shared_ptr<vec_watch_stream> speed() = 0;

	virtual boost::shared_ptr<vec_watch_stream> position() = 0;

	virtual servo_stream_ptr tilt_servo() = 0;

	virtual servo_stream_ptr yaw_servo() = 0;

	virtual servo_stream_ptr pitch_servo() = 0;

	virtual servo_stream_ptr gas_servo() = 0;

};

} // namepsace autopilot

#endif // PLAIN_COCKPIT_H
