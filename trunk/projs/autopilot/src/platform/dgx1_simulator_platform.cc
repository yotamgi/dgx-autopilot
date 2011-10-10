#include "dgx1_simulator_platform.h"
#include <stream/stream_importer.h>


namespace autopilot {

DGX1SimulatorPlatform::DGX1SimulatorPlatform(std::string address)
{
	stream::StreamImporter importer(address);

	m_gyro = importer.import_stream<stream::VecGenerator<float,3> >("simulator_gyro");
	m_acc = importer.import_stream<stream::VecGenerator<float,3> >("simulator_acc");
	m_compass = importer.import_stream<stream::VecGenerator<float,3> >("simulator_compass");
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

} //namespace autopilot
