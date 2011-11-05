#ifndef PLAIN_COCKPIT_H
#define PLAIN_COCKPIT_H

#include <stream/generators.h>
#include <stream/util/lin_algebra.h>
#include "interfaces/servo.h"

namespace autopilot {

/**
 * Normal Plain Cockpit.
 * Normal is every plain, which has orientation, speed and can be controlled
 * with 4 servo channels - yaw, pitch, roll and gas.
 */
class NormalPlainCockpit {
public:

	~NormalPlainCockpit() {}

	virtual boost::shared_ptr<stream::DataGenerator<lin_algebra::vec3f> > orientation() = 0;

	virtual stream::DataGenerator<float>* speed() = 0;

	virtual Servo* tilt_servo() = 0;

	virtual Servo* yaw_servo() = 0;

	virtual Servo* pitch_servo() = 0;

	virtual Servo* gas_servo() = 0;

};

} // namepsace autopilot

#endif // PLAIN_COCKPIT_H
