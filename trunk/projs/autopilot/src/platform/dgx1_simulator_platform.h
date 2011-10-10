#ifndef DGX1_SIMULATOR_PLATFORM_H_
#define DGX1_SIMULATOR_PLATFORM_H_

#include "interfaces/plain_platform.h"
#include <boost/shared_ptr.hpp>

namespace autopilot {

class DGX1SimulatorPlatform : public NormalPlainPlatform {
public:
	DGX1SimulatorPlatform(std::string address);

	boost::shared_ptr<stream::VecGenerator<float,3> > acc_sensor();

	boost::shared_ptr<stream::VecGenerator<float,3> > gyro_sensor();

	boost::shared_ptr<stream::VecGenerator<float,3> > compass_sensor();

	boost::shared_ptr<stream::VecGenerator<float,3> > gps_sensor();

	Servo* tilt_servo();

	Servo* yaw_servo();

	Servo* pitch_servo();

	Servo* gas_servo();

private:
	boost::shared_ptr<stream::VecGenerator<float,3 > > m_acc;
	boost::shared_ptr<stream::VecGenerator<float,3 > > m_gyro;
	boost::shared_ptr<stream::VecGenerator<float,3 > > m_compass;

};

} // namespace autopilot

#endif /* DGX1_SIMULATOR_PLATFORM_H_ */
