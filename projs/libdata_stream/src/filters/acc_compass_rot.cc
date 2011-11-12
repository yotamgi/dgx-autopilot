#include "acc_compass_rot.h"

namespace stream {
namespace filters {

static float lim(float a, float down, float up) {
	return std::max(std::min(a, up), down);
}

AccCompassRotation::AccCompassRotation(
		boost::shared_ptr<vec_stream_t> acc,
		boost::shared_ptr<vec_stream_t> compass):
	m_acc(acc),
	m_compass(compass),
	m_reliable_stream(new AccCompassReliable)
{
	lin_algebra::vec3f expected_north = m_compass->get_data();
	m_north_pitch_angle = std::asin(expected_north[1]/lin_algebra::vec_len(expected_north));
}

AccCompassRotation::AccCompassRotation(
		boost::shared_ptr<vec_stream_t> acc,
		boost::shared_ptr<vec_stream_t> compass,
		lin_algebra::vec3f expected_north):
	m_acc(acc),
	m_compass(compass),
	m_reliable_stream(new AccCompassReliable)

{
	m_north_pitch_angle = std::asin(expected_north[1]/lin_algebra::vec_len(expected_north)) * 180./lin_algebra::PI;
}


AccCompassRotation::AccCompassRotation(
		boost::shared_ptr<vec_stream_t> acc,
		boost::shared_ptr<vec_stream_t> compass,
		float north_pitch_angle):
	m_acc(acc),
	m_compass(compass),
	m_north_pitch_angle(north_pitch_angle),
	m_reliable_stream(new AccCompassReliable)
{}


lin_algebra::mat3f AccCompassRotation::get_data() {

	// get the sensor data
	lin_algebra::rowvec3f ground(m_acc->get_data().mem),
			north(m_compass->get_data().mem);

	// understand the reliability, and normalize the vectors
	float acc_len_closeness = fabs(1. - lin_algebra::vec_len(ground));
	float compass_len_closeness = fabs(20. - lin_algebra::vec_len(north))/20.;
	north = lin_algebra::normalize(north);
	ground = lin_algebra::normalize(ground);
	float angle = lin_algebra::angle_between(ground, north);
	float angle_closeness = fabs((m_north_pitch_angle - angle)/(m_north_pitch_angle));

	m_reliable_stream->reliability = lim(
			0.4  - (acc_len_closeness*6. + compass_len_closeness/5. + angle_closeness/50.),
			0., 1.
	);
	//std::cout << acc_len_closeness << ", " <<  compass_len_closeness << ", " << angle_closeness << " = " << m_reliable_stream->reliability << std::endl;
	//std::cout << angle << std::endl;

	// create the rotation matrix and fill its components
	lin_algebra::mat3f rot;

	// the y we know, so just set it
	rot.row(1) = -ground;

	// calculate z as the vector orthogonal to x and y
	lin_algebra::rowvec3f plane_z = lin_algebra::cross_product(north, ground);
	rot.row(2) = plane_z;

	// calculating the x using the north
	rot.row(0) = lin_algebra::cross_product(plane_z, ground);


	return rot;
}

} // namespace filters
} // namespace stream
