#ifndef DGX1_PLATFORM_H_
#define DGX1_PLATFORM_H_

#include "interfaces/plain_platform.h"
#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/connection.h>
#include <stream/stream_connection.h>
#include <stdexcept>

namespace autopilot {

typedef stream::filters::WatchFilter<lin_algebra::vec3f> 	vec3_watch_stream;
typedef stream::DataPopStream<lin_algebra::vec3f>			vec3_stream;
typedef boost::shared_ptr<vec3_stream> 						vec3_stream_ptr;
typedef stream::DataPushStream<float> 						servo_stream;


class DGX1Platform : public NormalPlainPlatform {
public:
	DGX1Platform();
	~DGX1Platform(){}

	typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > vec_stream_ptr;

	boost::shared_ptr<vec3_watch_stream> acc_sensor();

	boost::shared_ptr<vec3_watch_stream> gyro_sensor();

	boost::shared_ptr<vec3_watch_stream> compass_sensor();

	void register_pos_gps_reciever(gps_pos_reciever_ptr reciever);

	void register_gps_speed_dir_reciever(gps_speed_dir_reciever_ptr reciever);

	void register_gps_speed_mag_reciever(gps_speed_mag_reciever_ptr reciever);

	boost::shared_ptr<stream::DataPushStream<float> > tilt_servo();

	boost::shared_ptr<stream::DataPushStream<float> > yaw_servo();

	boost::shared_ptr<stream::DataPushStream<float> > pitch_servo();

	boost::shared_ptr<stream::DataPushStream<float> > gas_servo();

private:
	boost::shared_ptr<vec3_watch_stream> m_acc;
	boost::shared_ptr<vec3_watch_stream> m_gyro;
	boost::shared_ptr<vec3_watch_stream> m_compass;
};


class DGX1SimulatorPlatform : public NormalPlainPlatform {
public:
	DGX1SimulatorPlatform(boost::shared_ptr<stream::ConnectionFactory> conn);

	boost::shared_ptr<vec3_watch_stream> acc_sensor();

	boost::shared_ptr<vec3_watch_stream> gyro_sensor();

	boost::shared_ptr<vec3_watch_stream> compass_sensor();

	void register_pos_gps_reciever(gps_pos_reciever_ptr reciever);

	void register_gps_speed_dir_reciever(gps_speed_dir_reciever_ptr reciever);

	void register_gps_speed_mag_reciever(gps_speed_mag_reciever_ptr reciever);

	boost::shared_ptr<servo_stream> tilt_servo();

	boost::shared_ptr<servo_stream> yaw_servo();

	boost::shared_ptr<servo_stream> pitch_servo();

	boost::shared_ptr<servo_stream> gas_servo();

private:

	template <class T>
	class PushStreamForwarder : public stream::DataPushStream<T> {
	public:
		typedef boost::shared_ptr<stream::DataPushStream<T> > reciever_ptr;

		void set_reciever(reciever_ptr reciever) { m_reciever = reciever; }

		void set_data(const T& data) { if (m_reciever) m_reciever->set_data(data); }

	private:
		reciever_ptr m_reciever;
	};


	boost::shared_ptr<vec3_watch_stream> m_acc;
	boost::shared_ptr<vec3_watch_stream> m_gyro;
	boost::shared_ptr<vec3_watch_stream> m_compass;

	boost::shared_ptr<servo_stream> m_gas;
	boost::shared_ptr<servo_stream> m_pitch;
	boost::shared_ptr<servo_stream> m_tilt;
	boost::shared_ptr<servo_stream> m_yaw;

	boost::shared_ptr<PushStreamForwarder<lin_algebra::vec3f> > m_gps_pos_forwarder;
	boost::shared_ptr<PushStreamForwarder<float> > m_gps_speed_dir_forwarder;
	boost::shared_ptr<PushStreamForwarder<float> > m_gps_speed_mag_forwarder;

	stream::StreamConnection m_stream_conn;
};


} //namespace autopilot

#endif /* DGX1_PLATFORM_H_ */
