#include "dgx1_platform.h"
#include "hw/adxl345_acc.h"
#include "hw/itg3200_gyro.h"
#include "hw/hmc5843_compass.h"


namespace autopilot {

DGX1Platform::DGX1Platform():
		m_acc(new Adxl345Acc(2)),
		m_gyro(new Itg3200Gyro(2)),
		m_compass(new Hmc5843Compass(2))
{}

stream::VecGenerator<float,3>* DGX1Platform::acc_sensor() {
	return m_acc.get();
}

stream::VecGenerator<float,3>* DGX1Platform::gyro_sensor() {
	return m_gyro.get();
}

stream::VecGenerator<float,3>* DGX1Platform::compass_sensor() {
	return m_compass.get();
}

stream::VecGenerator<float,3>* DGX1Platform::gps_sensor() {
	throw std::logic_error("GPS not implemented yet on dgx1 platform");
	return NULL;
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
