#include "acc_compass_rot.h"

namespace stream {
namespace filters {

static float lim(float a, float down, float up) {
	return std::max(std::min(a, up), down);
}

AccCompassRotation::AccCompassRotation(
		boost::shared_ptr<VecGenerator<float,3> > acc,
		boost::shared_ptr<VecGenerator<float,3> > compass):
	m_acc(acc),
	m_compass(compass),
	m_reliable_stream(new AccCompassReliable)
{
	VecGenerator<float,3>::vector_t expected_north = m_compass->get_data();
	m_north_pitch_angle = std::asin(expected_north[1]/lin_algebra::vec_len(expected_north));
}

AccCompassRotation::AccCompassRotation(
		boost::shared_ptr<VecGenerator<float,3> > acc,
		boost::shared_ptr<VecGenerator<float,3> > compass,
		VecGenerator<float,3>::vector_t expected_north):
	m_acc(acc),
	m_compass(compass),
	m_reliable_stream(new AccCompassReliable)

{
	m_north_pitch_angle = std::asin(expected_north[1]/lin_algebra::vec_len(expected_north)) * 180./lin_algebra::PI;
}


AccCompassRotation::AccCompassRotation(
		boost::shared_ptr<VecGenerator<float,3> > acc,
		boost::shared_ptr<VecGenerator<float,3> > compass,
		float north_pitch_angle):
	m_acc(acc),
	m_compass(compass),
	m_north_pitch_angle(north_pitch_angle),
	m_reliable_stream(new AccCompassReliable)
{}


lin_algebra::matrix_t AccCompassRotation::get_data() {

	// get the sensor data
	typename VecGenerator<float,3>::vector_t ground_data, north_data;
	north_data = m_compass->get_data();
	ground_data = m_acc->get_data();


	// convert it to comfortable vectors
	lin_algebra::vector_t ground(3), north(3);
	north[0] = north_data[0]; 	north[1] = north_data[1]; 	north[2] = north_data[2];
	ground[0] = ground_data[0]; ground[1] = ground_data[1]; ground[2] = ground_data[2];

	// understand the reliability, and normalize the vectors
	float acc_len_closeness = fabs(1. - lin_algebra::vec_len(ground));
	float compass_len_closeness = fabs(32. - lin_algebra::vec_len(north));
	lin_algebra::normalize(north);
	lin_algebra::normalize(ground);
	float angle = lin_algebra::angle_between(ground, north);
	float angle_closeness = fabs((90.-m_north_pitch_angle - angle)/(90.-m_north_pitch_angle));

	m_reliable_stream->reliability = lim(
			0.3 - (acc_len_closeness + compass_len_closeness + angle_closeness)*3.,
			0., 1.
	);

	// create the rotation matrix and fill its components
	lin_algebra::matrix_t rot(3,3);

	// the y we know, so just set it
	lin_algebra::mat_col(rot, 1) = -ground;

	// calculating the x using the north
	north -= ground * std::sin(m_north_pitch_angle / 180.*lin_algebra::PI);
	lin_algebra::normalize(north);
	lin_algebra::mat_col(rot, 0) = -north;

	// calculate z as the vector orthogonal to x and y
	lin_algebra::mat_col(rot, 2) = lin_algebra::cross_product(north, ground);

	return rot;
}

} // namespace filters
} // namespace stream
