#include "fusion_filter.h"
#include <stream/filters/low_pass_filter.h>
#include <boost/make_shared.hpp>

namespace autopilot {

const size_t FusionFilter::ACC_LOW_PASS = 20;

static float lim(float a, float down, float up) {
	return std::max(std::min(a, up), down);
}

///////////////////////////////////////////////////////////////////////////////////
// Constructors and interface functions
///////////////////////////////////////////////////////////////////////////////////

FusionFilter::FusionFilter(boost::shared_ptr<vec_stream_t> acc,
						   boost::shared_ptr<vec_stream_t> compass,
						   boost::shared_ptr<vec_stream_t> gyro,
						   boost::shared_ptr<float_push_gen_t> speed):
			m_state(lin_algebra::identity_matrix<lin_algebra::mat3f>(3, 3)),
			m_acc(boost::make_shared<stream::filters::LowPassFilter<lin_algebra::vec3f> >(acc, ACC_LOW_PASS)),
			m_compass(compass),
			m_gyro(gyro),
			m_curr_speed(new stream::PushToPopConv<float>(0.)),
			m_reliable_stream(new stream::PushToPopConv<float>(0.)),
			m_rest_orientation(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f())),
			m_fixed_acc(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f())),
			m_calibration_data(lin_algebra::create_vec3f(0., 0., 0.))


{
	lin_algebra::vec3f expected_north = m_compass->get_data();
	m_north_pitch_angle = std::asin(expected_north[1]/lin_algebra::vec_len(expected_north));
	if (speed) {
		speed->register_receiver(m_curr_speed);
	}
}

FusionFilter::FusionFilter(boost::shared_ptr<vec_stream_t> acc,
						   boost::shared_ptr<vec_stream_t> compass,
						   boost::shared_ptr<vec_stream_t> gyro,
						   lin_algebra::vec3f expected_north,
						   boost::shared_ptr<float_push_gen_t> speed):
		m_state(lin_algebra::identity_matrix<lin_algebra::mat3f>(3, 3)),
		m_acc(boost::make_shared<stream::filters::LowPassFilter<lin_algebra::vec3f> >(acc, ACC_LOW_PASS)),
		m_compass(compass),
		m_gyro(gyro),
		m_curr_speed(new stream::PushToPopConv<float>(0.)),
		m_reliable_stream(new stream::PushToPopConv<float>(0.)),
		m_rest_orientation(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f())),
		m_fixed_acc(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f()))

{
	m_north_pitch_angle = std::asin(expected_north[1]/lin_algebra::vec_len(expected_north)) * 180./lin_algebra::PI;
	if (speed) {
		speed->register_receiver(m_curr_speed);
	}
}

FusionFilter::FusionFilter(boost::shared_ptr<vec_stream_t> acc,
						   boost::shared_ptr<vec_stream_t> compass,
						   boost::shared_ptr<vec_stream_t> gyro,
						   float north_pitch_angle,
						   boost::shared_ptr<float_push_gen_t> speed):
		m_state(lin_algebra::identity_matrix<lin_algebra::mat3f>(3, 3)),
		m_acc(boost::make_shared<stream::filters::LowPassFilter<lin_algebra::vec3f> >(acc, ACC_LOW_PASS)),
		m_compass(compass),
		m_gyro(gyro),
		m_curr_speed(new stream::PushToPopConv<float>(0.)),
		m_north_pitch_angle(north_pitch_angle),
		m_reliable_stream(new stream::PushToPopConv<float>(0.)),
		m_rest_orientation(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f())),
		m_fixed_acc(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f()))

{
	if (speed) {
		speed->register_receiver(m_curr_speed);
	}
}

lin_algebra::vec3f FusionFilter::get_data() {
	lin_algebra::vec3f acc_data 	;
	lin_algebra::vec3f compass_data ;
	lin_algebra::vec3f gyro_data 	;

	try	{
	acc_data = m_acc->get_data();
	} catch (...) {
		std::cout << "Acc failed.." << std::endl;
	} try	{
		compass_data = m_compass->get_data();
	} catch (...) {
		std::cout << "Compass failed.." << std::endl;
	} try	{
		gyro_data = m_gyro->get_data();
	} catch (...) {
		std::cout << "Gyro failed.." << std::endl;
	}

	// filter!
	lin_algebra::vec3f out = filter(acc_data, compass_data, gyro_data);

	// calibrate
	out += m_calibration_data;
	while (out[0] >  180.0f) out[0] -= 360.0f;
	while (out[0] < -180.0f) out[0] += 360.0f;
	while (out[1] >  360.0f) out[1] -= 360.0f;
	while (out[1] <    0.0f) out[1] += 360.0f;
	while (out[2] >  180.0f) out[2] -= 360.0f;
	while (out[3] < -180.0f) out[2] += 360.0f;

	return out;
}

void FusionFilter::calibrate(lin_algebra::vec3f calibration_data) {
	m_calibration_data = calibration_data;
}

///////////////////////////////////////////////////////////////////////////////////
// The actual Filter Function
///////////////////////////////////////////////////////////////////////////////////

lin_algebra::vec3f FusionFilter::filter(lin_algebra::vec3f acc_data,
						  lin_algebra::vec3f compass_data,
						  lin_algebra::vec3f gyro_data) {

	// calculate time delta
	float time_delta = m_timer.passed();
	m_timer.reset();

	if (time_delta > 0.1f) {
		time_delta = 0.1f;
	}

	if (m_curr_speed) {
		// fix the acc with the gyro data:
		acc_data[1] -= 0.05 * m_curr_speed->get_data() * gyro_data[0] * lin_algebra::PI / 180.;
		acc_data[0] += 0.05 * m_curr_speed->get_data() * gyro_data[1] * lin_algebra::PI / 180.;
	}

	// first, understand the orientation from the acc-compass
	lin_algebra::mat3f acc_compass_rot = acc_compass_orientation(acc_data, compass_data);

	// now, understand the reliability
	float reliability = understand_reliability(acc_data, compass_data);


	// save the data for debug resons
	m_reliable_stream->set_data(reliability);
	m_rest_orientation->set_data(matrix_to_euler(acc_compass_rot));
	m_fixed_acc->set_data(acc_data);

	// understand the av matrix from gyro
	lin_algebra::mat3f gyro_rot_mat = av_to_rot_mat(gyro_data);

	// fusion!
	float chane_factor = lim(reliability * time_delta, 0., 1.);
	m_state = (1.-chane_factor)*m_state + chane_factor*(acc_compass_rot - m_state);
	m_state = m_state + time_delta * m_state * gyro_rot_mat;

	m_state = maintain_orthonormal(m_state);

	return matrix_to_euler(m_state);
}

///////////////////////////////////////////////////////////////////////////////////
// Helpful methods
///////////////////////////////////////////////////////////////////////////////////

float FusionFilter::understand_reliability(lin_algebra::vec3f ground, lin_algebra::vec3f north) {
	// understand the reliability, and normalize the vectors
	float acc_len_closeness = fabs(1. - lin_algebra::vec_len(ground));
//	float compass_len_closeness = fabs(20. - lin_algebra::vec_len(north))/20.;
//	north = lin_algebra::normalize(north);
//	ground = lin_algebra::normalize(ground);
//
//	float angle = lin_algebra::angle_between(ground, north);
//	float angle_closeness = fabs((m_north_pitch_angle - angle)/(m_north_pitch_angle));
	if ((acc_len_closeness < 0.03) && (acc_len_closeness > -0.03)) {
		acc_len_closeness = 0.0;
	}

	return lim(
			1. - (acc_len_closeness*10.),// + compass_len_closeness/5. + angle_closeness/50.),
			0., 1.
	);
}

lin_algebra::mat3f FusionFilter::acc_compass_orientation(lin_algebra::vec3f ground_, lin_algebra::vec3f north_) {
	lin_algebra::rowvec3f ground(ground_.mem), north(north_.mem);

	// normalize vectors
	ground = lin_algebra::normalize(ground);
	north = lin_algebra::normalize(north);

	// create the rotation matrix and fill its components
	lin_algebra::mat3f rot;

	// the y we know, so just set it
	rot.row(1) = -ground;

	// calculate z as the vector orthogonal to x and y
	lin_algebra::rowvec3f plane_z = lin_algebra::normalize(lin_algebra::cross_product(north, ground));
	rot.row(2) = plane_z;

	// calculating the x using the north
	rot.row(0) = lin_algebra::normalize(lin_algebra::cross_product(plane_z, ground));

	return rot;
}

lin_algebra::vec3f FusionFilter::matrix_to_euler(lin_algebra::mat3f rot_mat) {
	lin_algebra::vec3f ans;

	// as simple as a formula
	ans[0] =  asin(rot_mat(1,2)) * 180. / lin_algebra::PI;
	ans[1] =  atan2(rot_mat(0,2), rot_mat(2,2)) * 180. / lin_algebra::PI;
	ans[2] = -atan2(rot_mat(1,0), rot_mat(1,1)) * 180. / lin_algebra::PI;

	return ans;
}

lin_algebra::mat3f FusionFilter::av_to_rot_mat(lin_algebra::vec3f av) {
	float wx = av[0] / 180. * lin_algebra::PI;
	float wy = av[1] / 180. * lin_algebra::PI;
	float wz = av[2] / 180. * lin_algebra::PI;

	lin_algebra::mat3f update; // = W + I
	update(0,0) = 0.;		update(0,1) = -1.*wz;	update(0,2) = wy;
	update(1,0) = wz;   	update(1,1) = 0.;   	update(1,2) = -1.*wx;
	update(2,0) = -1.*wy;  	update(2,1) = wx;   	update(2,2) = 0.;

	return  update;
}

lin_algebra::mat3f FusionFilter::maintain_orthonormal(lin_algebra::mat3f m)
{
	// maintain the matrix orthogonal
	m.col(2) = lin_algebra::cross_product<lin_algebra::vec3f>(m.col(0), m.col(1));
	m.col(0) = lin_algebra::cross_product<lin_algebra::vec3f>(m.col(1), m.col(2));

	// maintain the mtrix normal
	m.col(0) /= lin_algebra::vec_len(m.col(0));
	m.col(1) /= lin_algebra::vec_len(m.col(1));
	m.col(2) /= lin_algebra::vec_len(m.col(2));

	return m;
}


}  // namespace autopilot
