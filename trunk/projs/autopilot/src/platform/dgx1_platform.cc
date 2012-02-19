#include "dgx1_platform.h"
#include "hw/adxl345_acc.h"
#include "hw/itg3200_gyro.h"
#include "hw/hmc5843_compass.h"
#include <stream/stream_connection.h>
#include <stream/stream_utils.h>

#include <boost/make_shared.hpp>


namespace autopilot {

NormalPlainPlatform create_dgx1_platform() {
	NormalPlainPlatform dgx1_platform;
	dgx1_platform.acc_sensor.reset(new vec3_watch_stream(boost::make_shared<Adxl345Acc>(2)));
	dgx1_platform.gyro_sensor.reset(new vec3_watch_stream(boost::make_shared<Itg3200Gyro>(2)));
	dgx1_platform.compass_sensor.reset(new vec3_watch_stream(boost::make_shared<Hmc5843Compass>(2)));

	boost::shared_ptr<stream::PushForwarder<lin_algebra::vec3f> > pos_forwarder(new stream::PushForwarder<lin_algebra::vec3f>);
	boost::shared_ptr<stream::PushForwarder<float> > speed_dir_forwarder(new stream::PushForwarder<float>);
	boost::shared_ptr<stream::PushForwarder<float> > speed_mag_forwarder(new stream::PushForwarder<float>);

	static Gps gps_sensor;
	gps_sensor.set_pos_reciever_stream(pos_forwarder);
	gps_sensor.set_speed_dir_reciever_stream(speed_dir_forwarder);
	gps_sensor.set_speed_mag_reciever_stream(speed_mag_forwarder);

	dgx1_platform.gps_pos_generator = pos_forwarder;
	dgx1_platform.gps_speed_mag_generator = speed_mag_forwarder;
	dgx1_platform.gps_speed_dir_generator = speed_dir_forwarder;

	return dgx1_platform;
}


NormalPlainPlatform create_dgx1_simulator_platform(boost::shared_ptr<stream::ConnectionFactory> conn_factory)
{
	static stream::StreamConnection conn(conn_factory);

	// gps
	boost::shared_ptr<stream::PushForwarder<lin_algebra::vec3f> > pos_forwarder(new stream::PushForwarder<lin_algebra::vec3f>);
	boost::shared_ptr<stream::PushForwarder<float> > speed_dir_forwarder(new stream::PushForwarder<float>);
	boost::shared_ptr<stream::PushForwarder<float> > speed_mag_forwarder(new stream::PushForwarder<float>);
	conn.export_push_stream<lin_algebra::vec3f>(pos_forwarder, std::string("gps_pos_reciever"));
	conn.export_push_stream<float>(speed_dir_forwarder, std::string("gps_speed_dir_reciever"));
	conn.export_push_stream<float>(speed_mag_forwarder, std::string("gps_speed_mag_reciever"));
	conn.run();

	// create the platform and fill it
	NormalPlainPlatform dgx1_platform;

	dgx1_platform.acc_sensor.reset(new vec3_watch_stream(conn.import_pop_stream<lin_algebra::vec3f>("simulator_acc")));
	dgx1_platform.gyro_sensor.reset(new vec3_watch_stream(conn.import_pop_stream<lin_algebra::vec3f>("simulator_gyro")));
	dgx1_platform.compass_sensor.reset(new vec3_watch_stream(conn.import_pop_stream<lin_algebra::vec3f>("simulator_compass")));

	dgx1_platform.pitch_servo = conn.import_push_stream<float>("simulator_pitch_servo");
	dgx1_platform.tilt_servo = conn.import_push_stream<float>("simulator_tilt_servo");
	dgx1_platform.gas_servo = conn.import_push_stream<float>("simulator_gas_servo");

	dgx1_platform.gps_pos_generator = pos_forwarder;
	dgx1_platform.gps_speed_dir_generator = speed_dir_forwarder;
	dgx1_platform.gps_speed_mag_generator = speed_mag_forwarder;


	return dgx1_platform;
}

} //namespace autopilot
