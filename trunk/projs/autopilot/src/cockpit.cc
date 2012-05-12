#include "cockpit.h"
#include <stream/util/lin_algebra.h>
#include <stream/filters/integral_filter.h>
#include <stream/filters/matrix_to_euler_filter.h>
#include <stream/filters/acc_compass_rot.h>
#include <stream/filters/gyro_to_av_matrix.h>
#include <stream/filters/low_pass_filter.h>
#include <stream/func_filter.h>
#include <boost/make_shared.hpp>
#include <boost/bind/bind.hpp>

namespace autopilot {

static lin_algebra::mat3f update_matrix(const lin_algebra::mat3f& m1,
		const lin_algebra::mat3f& m2)
{

	lin_algebra::mat3f rot = m1 + m1 * m2;

	// maintain the matrix orthogonal
	rot.col(2) = lin_algebra::cross_product<lin_algebra::vec3f>(rot.col(0), rot.col(1));
	rot.col(0) = lin_algebra::cross_product<lin_algebra::vec3f>(rot.col(1), rot.col(2));

	// maintain the mtrix normal
	rot.col(0) /= lin_algebra::vec_len(rot.col(0));
	rot.col(1) /= lin_algebra::vec_len(rot.col(1));
	rot.col(2) /= lin_algebra::vec_len(rot.col(2));

	return rot;
}

Cockpit::Cockpit(NormalPlainPlatform platform):
		m_platform(platform),
		m_gps_pos(boost::make_shared<stream::PushToPopConv<lin_algebra::vec3f> >(lin_algebra::vec3f())),
		m_gps_speed_dir(boost::make_shared<stream::PushToPopConv<float> >(0.)),
		m_gps_speed_mag(boost::make_shared<stream::PushToPopConv<float> >(0.))
{
	// This is the stream schematics:
	//
	//				  +----GyroToAv----IntegralFilter----MatrixToEulerFilter---- m_gyro_orientation
	//         (watch)|
	// gyro ----------+--------------------GyroToAv-------+
	//			    				   				      |
	// acc --------+       orientation+----WatchFilter----+ KalmanFilter----MatrixToEulerFilter----WatchFilter----m_orientation
	//			   | AccCompassRot    |		  +			  |
	// compass ----+	   reliability+----WatchFilter----+
	//                                        |    +
	//                                        |    +------------ m_rest_orientation
	//								          +----------------- m_rest_reliability
	//

	namespace filter = stream::filters;

	// add watch to the gyro
	boost::shared_ptr<vec3_watch_stream> gyro_watch(new vec3_watch_stream(m_platform.gyro_sensor));
	m_platform.gyro_sensor = gyro_watch;

	m_platform.gps_pos_generator->register_receiver(m_gps_pos);
	m_platform.gps_speed_dir_generator->register_receiver(m_gps_speed_dir);
	m_platform.gps_speed_mag_generator->register_receiver(m_gps_speed_mag);

	m_gyro_orientation = boost::make_shared<filter::MatrixToEulerFilter>(
		boost::make_shared<filter::IntegralFilter<lin_algebra::mat3f> >(
			boost::make_shared<filter::GyroToAVMatrix>(
				gyro_watch->get_watch_stream()
			),
			lin_algebra::identity_matrix<lin_algebra::mat3f>(3u, 3u),
			update_matrix
		)
	);

	boost::shared_ptr<FusionFilter> fusion_filter = boost::make_shared<FusionFilter>(
					m_platform.acc_sensor,
					m_platform.compass_sensor,
					m_platform.gyro_sensor,
					20., // the north explected angle
					m_gps_speed_mag
	);

	m_orientation = boost::make_shared<vec3_watch_stream>(fusion_filter);
	m_rest_reliability = fusion_filter->get_reliability_stream();
	m_rest_orientation = fusion_filter->get_rest_orientation_stream();
	m_fixed_acc = fusion_filter->get_fixed_acc_stream();
}

boost::shared_ptr<vec3_stream> Cockpit::watch_gyro_orientation() {
	return m_gyro_orientation;
}
boost::shared_ptr<vec3_stream> Cockpit::watch_rest_orientation() {
	return m_rest_orientation;
}

boost::shared_ptr<vec3_watch_stream> Cockpit::orientation() {
	return m_orientation;
}

boost::shared_ptr<float_stream> Cockpit::watch_rest_reliability() {
	return m_rest_reliability;
}

boost::shared_ptr<vec3_stream> 	Cockpit::watch_fixed_acc() {
	return m_fixed_acc;
}

boost::shared_ptr<float_watch_stream> Cockpit::ground_speed() {
	return boost::make_shared<float_watch_stream>(
			(boost::shared_ptr<stream::DataPopStream<float> >)m_gps_speed_mag
	);
}

boost::shared_ptr<vec2_watch_stream> Cockpit::position() {
	return boost::make_shared<vec2_watch_stream>(
			stream::create_func_pop_filter<lin_algebra::vec3f,lin_algebra::vec2f> (
					m_gps_pos,
					lin_algebra::get<0, 1>
			)
	);
}

boost::shared_ptr<float_watch_stream> Cockpit::alt() {
	return boost::make_shared<float_watch_stream>(
			stream::create_func_pop_filter<lin_algebra::vec3f,float>(
				m_gps_pos,
				lin_algebra::get<2>
		)
	);
}

servo_stream_ptr Cockpit::tilt_servo() {
	return m_platform.tilt_servo;
}

servo_stream_ptr Cockpit::yaw_servo() {
	return m_platform.yaw_servo;
}

servo_stream_ptr Cockpit::pitch_servo(){
	return m_platform.pitch_servo;
}

servo_stream_ptr Cockpit::gas_servo() {
	return m_platform.gas_servo;
}


} // namespace autopilot
