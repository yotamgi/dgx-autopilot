#include "dgx1_simulator_platform.h"

namespace autopilot {

DGX1SimulatorPlatform::DGX1SimulatorPlatform(std::string address):m_importer(address)
{
	m_gyro = m_importer.import_stream<stream::VecGenerator<float,3> >("simulator_gyro");
	m_acc = m_importer.import_stream<stream::VecGenerator<float,3> >("simulator_acc");
	m_compass = m_importer.import_stream<stream::VecGenerator<float,3> >("simulator_compass");
}

boost::shared_ptr<stream::VecGenerator<float,3> > DGX1SimulatorPlatform::acc_sensor() {
	return m_acc;
}

boost::shared_ptr<stream::VecGenerator<float,3> > DGX1SimulatorPlatform::gyro_sensor() {
	return m_gyro;
}

boost::shared_ptr<stream::VecGenerator<float,3> > DGX1SimulatorPlatform::compass_sensor() {
	return m_compass;
}

boost::shared_ptr<stream::VecGenerator<float,3> > DGX1SimulatorPlatform::gps_sensor() {
	throw std::logic_error("GPS not implemented yet on dgx1 platform");
	return boost::shared_ptr<stream::VecGenerator<float,3> >();
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
