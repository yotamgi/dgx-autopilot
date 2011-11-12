#include "dgx1_platform.h"
#include "hw/adxl345_acc.h"
#include "hw/itg3200_gyro.h"
#include "hw/hmc5843_compass.h"

#include <boost/make_shared.hpp>


namespace autopilot {

DGX1Platform::DGX1Platform():
		m_acc(new vec_watch_stream(boost::make_shared<Adxl345Acc>(2))),
		m_gyro(new vec_watch_stream(boost::make_shared<Itg3200Gyro>(2))),
		m_compass(new vec_watch_stream(boost::make_shared<Hmc5843Compass>(2)))
{}

boost::shared_ptr<vec_watch_stream> DGX1Platform::acc_sensor() {
	return m_acc;
}

boost::shared_ptr<vec_watch_stream> DGX1Platform::gyro_sensor() {
	return m_gyro;
}

boost::shared_ptr<vec_watch_stream> DGX1Platform::compass_sensor() {
	return m_compass;
}

boost::shared_ptr<vec_watch_stream> DGX1Platform::gps_sensor() {
	throw std::logic_error("GPS not implemented yet on dgx1 platform");
	return boost::shared_ptr<vec_watch_stream>();
}

Servo* DGX1Platform::tilt_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return NULL;
}

Servo* DGX1Platform::yaw_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return NULL;
}

Servo* DGX1Platform::pitch_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return NULL;
}

Servo* DGX1Platform::gas_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return NULL;
}


} //namespace autopilot
