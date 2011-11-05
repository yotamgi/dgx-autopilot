#include "dgx1_simulator_platform.h"

namespace autopilot {

DGX1SimulatorPlatform::DGX1SimulatorPlatform(std::string address):m_importer(address)
{
	m_gyro = m_importer.import_stream<vec_stream>("simulator_gyro");
	m_acc = m_importer.import_stream<vec_stream>("simulator_acc");
	m_compass = m_importer.import_stream<vec_stream>("simulator_compass");
}

vec_stream_ptr DGX1SimulatorPlatform::acc_sensor() {
	return m_acc;
}

vec_stream_ptr DGX1SimulatorPlatform::gyro_sensor() {
	return m_gyro;
}

vec_stream_ptr DGX1SimulatorPlatform::compass_sensor() {
	return m_compass;
}

vec_stream_ptr DGX1SimulatorPlatform::gps_sensor() {
	throw std::logic_error("GPS not implemented yet on dgx1 platform");
	return vec_stream_ptr();
}

Servo* DGX1SimulatorPlatform::tilt_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return NULL;
}

Servo* DGX1SimulatorPlatform::yaw_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return NULL;
}

Servo* DGX1SimulatorPlatform::pitch_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return NULL;
}

Servo* DGX1SimulatorPlatform::gas_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return NULL;
}


} //namespace autopilot
