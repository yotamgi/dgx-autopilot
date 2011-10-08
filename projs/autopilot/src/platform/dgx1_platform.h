#ifndef DGX1_PLATFORM_H_
#define DGX1_PLATFORM_H_

#include "interfaces/plain_platform.h"
#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace autopilot {

class DGX1Platform : public NormalPlainPlatform {
public:
	DGX1Platform();

	stream::VecGenerator<float,3>* acc_sensor();

	stream::VecGenerator<float,3>* gyro_sensor();

	stream::VecGenerator<float,3>* compass_sensor();

	stream::VecGenerator<float,3>* gps_sensor();

	Servo* tilt_servo();

	Servo* yaw_servo();

	Servo* pitch_servo();

	Servo* gas_servo();

private:
	boost::shared_ptr<stream::VecGenerator<float,3> > m_acc;
	boost::shared_ptr<stream::VecGenerator<float,3> > m_gyro;
	boost::shared_ptr<stream::VecGenerator<float,3> > m_compass;
};

} //namespace autopilot

#endif /* DGX1_PLATFORM_H_ */
