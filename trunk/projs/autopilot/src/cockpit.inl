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

static lin_algebra::matrix_t update_matrix(const lin_algebra::matrix_t& m1,
		const lin_algebra::matrix_t& m2)
{

	lin_algebra::matrix_t rot = m1 + m1 * m2;

	// maintain the matrix ortho-normal
	lin_algebra::mat_col col0 = lin_algebra::mat_col(rot, 0);
	lin_algebra::mat_col col1 = lin_algebra::mat_col(rot, 1);
	lin_algebra::mat_col col2 = lin_algebra::mat_col(rot, 2);

	col2 = lin_algebra::cross_product(col0, col1);
	col0 = lin_algebra::cross_product(col1, col2);

	lin_algebra::normalize(col0);
	lin_algebra::normalize(col1);
	lin_algebra::normalize(col2);

	return rot;
}

inline Cockpit::Cockpit(boost::shared_ptr<NormalPlainPlatform> platform):
		m_platform(platform)
{
	namespace filter = stream::filters;

	m_gyro_orientation = boost::make_shared<filter::IntegralFilter<lin_algebra::matrix_t> >(
		boost::make_shared<filter::GyroToAVMatrix>(
			m_platform->gyro_sensor()
		),
		(lin_algebra::matrix_t)lin_algebra::identity_matrix<float>(3),
		update_matrix
	);

	boost::shared_ptr<filter::AccCompassRotation> acc_compass = boost::make_shared<filter::AccCompassRotation>(
		boost::make_shared<stream::filters::LowPassVecFilter<float,3> >(
				m_platform->acc_sensor(),
				1
		),
		m_platform->compass_sensor(),
		20. // the north explected angle
	);
	m_rest_orientation = acc_compass;
	m_rest_reliability = acc_compass->reliable_stream();

	m_orientation = boost::make_shared<filter::KalmanFilter<lin_algebra::matrix_t> >(
			boost::make_shared<filter::GyroToAVMatrix>(
				m_platform->gyro_sensor()
			),
			m_rest_orientation,
			m_rest_reliability,
			(lin_algebra::matrix_t)lin_algebra::identity_matrix<float>(3),
			update_matrix
	);
}

inline boost::shared_ptr<stream::VecGenerator<float,3> > Cockpit::orientation_gyro() {
	return boost::shared_ptr<stream::VecGenerator<float,3> >(
			(stream::VecGenerator<float,3>*)new stream::filters::MatrixToEulerFilter(m_gyro_orientation)
	);
}
inline boost::shared_ptr<stream::VecGenerator<float,3> > Cockpit::orientation_rest() {
	return boost::shared_ptr<stream::VecGenerator<float,3> >(
			(stream::VecGenerator<float,3>*)new stream::filters::MatrixToEulerFilter(m_rest_orientation)
	);
}

inline boost::shared_ptr<stream::VecGenerator<float,3> > Cockpit::orientation() {
	return boost::shared_ptr<stream::VecGenerator<float,3> >(
			(stream::VecGenerator<float,3>*)new stream::filters::MatrixToEulerFilter(m_orientation)
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
