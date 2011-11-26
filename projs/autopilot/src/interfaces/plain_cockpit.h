#ifndef PLAIN_COCKPIT_H
#define PLAIN_COCKPIT_H

#include <stream/data_pop_stream.h>
#include <stream/filters/watch_filter.h>
#include <stream/util/lin_algebra.h>
#include "interfaces/servo.h"

namespace autopilot {

typedef stream::DataPopStream<lin_algebra::vec3f> 			vec_stream;
typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec_watch_stream;
typedef stream::DataPopStream<float> 						float_stream;

/**
 * Normal Plain Cockpit.
 * Normal is every plain, which has orientation, speed and can be controlled
 * with 4 servo channels - yaw, pitch, roll and gas.
 */
class NormalPlainCockpit {
public:

	~NormalPlainCockpit() {}

	virtual boost::shared_ptr<vec_watch_stream> orientation() = 0;

	virtual stream::DataPopStream<float>* speed() = 0;

	virtual Servo* tilt_servo() = 0;

	virtual Servo* yaw_servo() = 0;

	virtual Servo* pitch_servo() = 0;

	virtual Servo* gas_servo() = 0;

};

} // namepsace autopilot

#endif // PLAIN_COCKPIT_H
