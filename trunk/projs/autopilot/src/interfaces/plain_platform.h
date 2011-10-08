#ifndef PLAIN_PLATFORM_H_
#define PLAIN_PLATFORM_H_

#include <stream/generators.h>
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

	virtual ~NormalPlainPlatform() {}

	virtual stream::VecGenerator<float,3>* acc_sensor() = 0;

	virtual stream::VecGenerator<float,3>* gyro_sensor() = 0;

	virtual stream::VecGenerator<float,3>* compass_sensor() = 0;

	virtual stream::VecGenerator<float,3>* gps_sensor() = 0;

	virtual Servo* tilt_servo() = 0;

	virtual Servo* yaw_servo() = 0;

	virtual Servo* pitch_servo() = 0;

	virtual Servo* gas_servo() = 0;
};

} //namespace autopilot


#endif // PLAIN_PLATFORM_H_
