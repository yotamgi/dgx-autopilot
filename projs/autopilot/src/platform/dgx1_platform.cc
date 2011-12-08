#include "dgx1_platform.h"
#include "hw/adxl345_acc.h"
#include "hw/itg3200_gyro.h"
#include "hw/hmc5843_compass.h"
#include <stream/stream_connection.h>

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

boost::shared_ptr<stream::DataPushStream<float> > DGX1Platform::tilt_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return boost::shared_ptr<stream::DataPushStream<float> >();
}

boost::shared_ptr<stream::DataPushStream<float> > DGX1Platform::yaw_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return boost::shared_ptr<stream::DataPushStream<float> >();
}

boost::shared_ptr<stream::DataPushStream<float> > DGX1Platform::pitch_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return boost::shared_ptr<stream::DataPushStream<float> >();
}

boost::shared_ptr<stream::DataPushStream<float> > DGX1Platform::gas_servo() {
	throw std::logic_error("Servos not implemented yet on dgx1 platform");
	return boost::shared_ptr<stream::DataPushStream<float> >();
}


//
// The simulator platform methods
//

DGX1SimulatorPlatform::DGX1SimulatorPlatform(boost::shared_ptr<stream::ConnectionFactory> conn):m_stream_conn(conn)
{
	m_gyro = boost::make_shared<vec_watch_stream>(m_stream_conn.import_pop_stream<lin_algebra::vec3f>("simulator_gyro"));
	m_acc = boost::make_shared<vec_watch_stream>(m_stream_conn.import_pop_stream<lin_algebra::vec3f>("simulator_acc"));
	m_compass = boost::make_shared<vec_watch_stream>(m_stream_conn.import_pop_stream<lin_algebra::vec3f>("simulator_compass"));

	m_pitch = m_stream_conn.import_push_stream<float>("simulator_pitch_servo");
	m_tilt = m_stream_conn.import_push_stream<float>("simulator_tilt_servo");
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

boost::shared_ptr<stream::DataPushStream<float> > DGX1SimulatorPlatform::tilt_servo() {
	return m_tilt;
}

boost::shared_ptr<stream::DataPushStream<float> > DGX1SimulatorPlatform::yaw_servo() {
	return m_yaw;
}

boost::shared_ptr<stream::DataPushStream<float> > DGX1SimulatorPlatform::pitch_servo() {
	return m_pitch;
}

boost::shared_ptr<stream::DataPushStream<float> > DGX1SimulatorPlatform::gas_servo() {
	return m_gas;
}


} //namespace autopilot
