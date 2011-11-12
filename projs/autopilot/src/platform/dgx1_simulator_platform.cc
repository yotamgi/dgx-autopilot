#include "dgx1_simulator_platform.h"
#include <boost/make_shared.hpp>

namespace autopilot {

DGX1SimulatorPlatform::DGX1SimulatorPlatform(std::string address):m_importer(address)
{
	m_gyro = boost::make_shared<vec_watch_stream>(m_importer.import_stream<vec_stream>("simulator_gyro"));
	m_acc = boost::make_shared<vec_watch_stream>(m_importer.import_stream<vec_stream>("simulator_acc"));
	m_compass = boost::make_shared<vec_watch_stream>(m_importer.import_stream<vec_stream>("simulator_compass"));
}

boost::shared_ptr<vec_watch_stream> DGX1SimulatorPlatform::acc_sensor() {
	return m_acc;
}

boost::shared_ptr<vec_watch_stream> DGX1SimulatorPlatform::gyro_sensor() {
	return m_gyro;
}

boost::shared_ptr<vec_watch_stream> DGX1SimulatorPlatform::compass_sensor() {
	return m_compass;
}

boost::shared_ptr<vec_watch_stream> DGX1SimulatorPlatform::gps_sensor() {
	throw std::logic_error("GPS not implemented yet on dgx1 platform");
	return boost::shared_ptr<vec_watch_stream>();
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
