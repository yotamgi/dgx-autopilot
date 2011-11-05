#include "cockpit.h"
#include <stream/util/lin_algebra.h>
#include <stream/filters/integral_filter.h>
#include <stream/filters/matrix_to_euler_filter.h>
#include <stream/filters/acc_compass_rot.h>
#include <stream/filters/gyro_to_av_matrix.h>
#include <stream/filters/kalman_filter.h>
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
		m_platform(platform)
{
	namespace filter = stream::filters;

	m_gyro_orientation = boost::make_shared<filter::IntegralFilter<lin_algebra::mat3f> >(
		boost::make_shared<filter::GyroToAVMatrix>(
			m_platform->gyro_sensor()
		),
		lin_algebra::identity_matrix<lin_algebra::mat3f>(3u, 3u),
		update_matrix
	);

	boost::shared_ptr<filter::AccCompassRotation> acc_compass = boost::make_shared<filter::AccCompassRotation>(
		boost::make_shared<stream::filters::LowPassFilter<lin_algebra::vec3f> >(
				m_platform->acc_sensor(),
				1
		),
		m_platform->compass_sensor(),
		20. // the north explected angle
	);
	m_rest_orientation = acc_compass;
	m_rest_reliability = acc_compass->reliable_stream();

	m_orientation = boost::make_shared<filter::KalmanFilter<lin_algebra::mat3f> >(
			boost::make_shared<filter::GyroToAVMatrix>(
				m_platform->gyro_sensor()
			),
			m_rest_orientation,
			m_rest_reliability,
			lin_algebra::identity_matrix<lin_algebra::mat3f>(3, 3),
			update_matrix
	);
}

inline boost::shared_ptr<stream::DataGenerator<lin_algebra::vec3f> > Cockpit::orientation_gyro() {
	return boost::shared_ptr<stream::DataGenerator<lin_algebra::vec3f> >(
			(stream::DataGenerator<lin_algebra::vec3f>*)new stream::filters::MatrixToEulerFilter(m_gyro_orientation)
	);
}
inline boost::shared_ptr<stream::DataGenerator<lin_algebra::vec3f> > Cockpit::orientation_rest() {
	return boost::shared_ptr<stream::DataGenerator<lin_algebra::vec3f> >(
			(stream::DataGenerator<lin_algebra::vec3f>*)new stream::filters::MatrixToEulerFilter(m_rest_orientation)
	);
}

inline boost::shared_ptr<stream::DataGenerator<lin_algebra::vec3f> > Cockpit::orientation() {
	return boost::shared_ptr<stream::DataGenerator<lin_algebra::vec3f> >(
			(stream::DataGenerator<lin_algebra::vec3f>*)new stream::filters::MatrixToEulerFilter(m_orientation)
	);
}

inline boost::shared_ptr<stream::DataGenerator<float> > Cockpit::rest_reliablity() {
	return m_rest_reliability;
}

inline stream::DataGenerator<float>* Cockpit::speed() {
	throw std::logic_error("speed not implemented yet on Cockpit");
	return NULL;
}

inline Servo* Cockpit::tilt_servo() {
	return m_platform->tilt_servo();
}

inline Servo* Cockpit::yaw_servo() {
	return m_platform->yaw_servo();
}

inline Servo* Cockpit::pitch_servo(){
	return m_platform->pitch_servo();
}

inline Servo* Cockpit::gas_servo() {
	return m_platform->gas_servo();
}


} // namespace autopilot
