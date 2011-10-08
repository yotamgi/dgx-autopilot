#ifndef __SERVO_H__
#define __SERVO_H__

#include <stdexcept>

class ServoException : public std::runtime_error {
public: 
	ServoException(std::string _what): runtime_error(_what) {}
};

class Servo {
public:

	/**
	 * Sets the servo state/angle.
	 *
	 * @ param statePercentage - a float number between 0 and 100 that
	 * 		determines the state of the servo.
	 * @ throws - exceptions if it failes.
	 */
	virtual void set_state(float statePercentage) = 0;
};

#endif // __SERVO_H__
