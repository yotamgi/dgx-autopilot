#include "cockpit.h"
#include <stream/util/lin_algebra.h>
#include <stream/filters/integral_filter.h>
#include <stream/filters/matrix_to_euler_filter.h>
#include <stream/filters/acc_compass_rot.h>
#include <stream/filters/gyro_to_av_matrix.h>
#include <stream/filters/low_pass_filter.h>
#include <boost/make_shared.hpp>

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

inline Cockpit::Cockpit(boost::shared_ptr<NormalPlainPlatform> platform):
		m_platform(platform),
		m_gps_filter(boost::make_shared<SimpleGpsFilter>(3))
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

	m_platform->register_gps_reciever(m_gps_filter);

	m_gyro_orientation = boost::make_shared<filter::MatrixToEulerFilter>(
		boost::make_shared<filter::IntegralFilter<lin_algebra::mat3f> >(
			boost::make_shared<filter::GyroToAVMatrix>(
				m_platform->gyro_sensor()->get_watch_stream()
			),
			lin_algebra::identity_matrix<lin_algebra::mat3f>(3u, 3u),
			update_matrix
		)
	);

	boost::shared_ptr<FusionFilter> fusion_filter = boost::make_shared<FusionFilter>(
					m_platform->acc_sensor(),
					m_platform->compass_sensor(),
					m_platform->gyro_sensor(),
					20., // the north explected angle
					m_gps_filter->get_speed_stream()
	);

	m_orientation = boost::make_shared<vec3_watch_stream>(fusion_filter);
	m_rest_reliability = fusion_filter->get_reliability_stream();
	m_rest_orientation = fusion_filter->get_rest_orientation_stream();
	m_fixed_acc = fusion_filter->get_fixed_acc_stream();
}

inline boost::shared_ptr<vec3_stream> Cockpit::watch_gyro_orientation() {
	return m_gyro_orientation;
}
inline boost::shared_ptr<vec3_stream> Cockpit::watch_rest_orientation() {
	return m_rest_orientation;
}

inline boost::shared_ptr<vec3_watch_stream> Cockpit::orientation() {
	return m_orientation;
}

inline boost::shared_ptr<float_stream> Cockpit::watch_rest_reliability() {
	return m_rest_reliability;
}

inline boost::shared_ptr<vec3_stream> 	Cockpit::watch_fixed_acc() {
	return m_fixed_acc;
}

inline boost::shared_ptr<vec3_watch_stream> Cockpit::speed() {
	return boost::make_shared<vec3_watch_stream>(m_gps_filter->get_speed_stream());
}

inline boost::shared_ptr<vec2_watch_stream> Cockpit::position() {
	return boost::make_shared<vec2_watch_stream>(m_gps_filter->get_position_stream());
}

inline boost::shared_ptr<float_watch_stream> Cockpit::alt() {
	return boost::make_shared<float_watch_stream>(m_gps_filter->get_alt_stream());
}

inline servo_stream_ptr Cockpit::tilt_servo() {
	return m_platform->tilt_servo();
}

inline servo_stream_ptr Cockpit::yaw_servo() {
	return m_platform->yaw_servo();
}

inline servo_stream_ptr Cockpit::pitch_servo(){
	return m_platform->pitch_servo();
}

inline servo_stream_ptr Cockpit::gas_servo() {
	return m_platform->gas_servo();
}


} // namespace autopilot