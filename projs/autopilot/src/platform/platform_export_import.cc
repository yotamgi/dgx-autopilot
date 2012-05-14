#include "platform_export_import.h"
#include <stream/stream_connection.h>
#include <stream/stream_utils.h>

namespace autopilot {

void export_platform(boost::shared_ptr<NormalPlainPlatform> platform,
		boost::shared_ptr<stream::ConnectionFactory> conn_factory)
{
	typedef stream::DataPopStream<lin_algebra::vec3f> vec3stream;
	typedef stream::DataPopStream<float> floatstream;
	typedef boost::shared_ptr<vec3stream> vec3stream_ptr;
	typedef boost::shared_ptr<floatstream> floatstream_ptr;

	stream::StreamConnection conn(conn_factory);
	conn.export_pop_stream<lin_algebra::vec3f>(platform->gyro_sensor, "gyro");
	conn.export_pop_stream<lin_algebra::vec3f>(platform->acc_sensor, "acc");
	conn.export_pop_stream<lin_algebra::vec3f>(platform->compass_sensor, "compass");
	//conn.export_pop_stream<float>(platform->alt, "simulator_alt");

	conn.export_push_stream<float>(platform->pitch_servo, "pitch_servo");
	conn.export_push_stream<float>(platform->yaw_servo, "yaw_servo");
	conn.export_push_stream<float>(platform->tilt_servo, "tilt_servo");
	conn.export_push_stream<float>(platform->gas_servo, "gas_servo");
	conn.run(true);

	platform->gps_pos_generator->register_receiver(conn.import_push_stream<lin_algebra::vec3f>("gps_pos_reciever"));
	platform->gps_speed_dir_generator->register_receiver(conn.import_push_stream<float>("gps_speed_dir_reciever"));
	platform->gps_speed_mag_generator->register_receiver(conn.import_push_stream<float>("gps_speed_mag_reciever"));

	while (true) usleep(10000000);
}

void nada() {}

boost::shared_ptr<NormalPlainPlatform> import_platform(boost::shared_ptr<stream::ConnectionFactory> conn_factory) {

	typedef stream::PushForwarder<lin_algebra::vec3f> vec3_push_forwarder;
	typedef stream::PushForwarder<float> float_push_forwarder;

	static stream::StreamConnection conn(conn_factory);

	// gps
	boost::shared_ptr<vec3_push_forwarder> pos_forwarder(new vec3_push_forwarder);
	boost::shared_ptr<float_push_forwarder> speed_dir_forwarder(new float_push_forwarder);
	boost::shared_ptr<float_push_forwarder> speed_mag_forwarder(new float_push_forwarder);
	conn.export_push_stream<lin_algebra::vec3f>(pos_forwarder, std::string("gps_pos_reciever"));
	conn.export_push_stream<float>(speed_dir_forwarder, std::string("gps_speed_dir_reciever"));
	conn.export_push_stream<float>(speed_mag_forwarder, std::string("gps_speed_mag_reciever"));
	conn.run();

	// create the platform and fill it
	boost::shared_ptr<NormalPlainPlatform> platform = boost::make_shared<NormalPlainPlatform>();

	platform->acc_sensor = conn.import_pop_stream<lin_algebra::vec3f>("acc");
	platform->gyro_sensor = conn.import_pop_stream<lin_algebra::vec3f>("gyro");
	platform->compass_sensor = conn.import_pop_stream<lin_algebra::vec3f>("compass");

	platform->pitch_servo = conn.import_push_stream<float>("pitch_servo");
	platform->tilt_servo = conn.import_push_stream<float>("tilt_servo");
	platform->gas_servo = conn.import_push_stream<float>("gas_servo");

	platform->gps_pos_generator = pos_forwarder;
	platform->gps_speed_dir_generator = speed_dir_forwarder;
	platform->gps_speed_mag_generator = speed_mag_forwarder;

	platform->alive = nada;

	return platform;
}

}  // namespace autopilot
