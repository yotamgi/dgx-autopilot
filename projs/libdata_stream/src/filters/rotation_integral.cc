
#include "rotation_integral.h"
#include <cmath>
#include "stream/util/time.h"

namespace stream {
namespace filters {

RotationIntegral::RotationIntegral(VecGenerator<float,3>* data_gen):
	DataFilter<typename VecGenerator<float,3>::vector_t, lin_algebra::matrix_t>(data_gen),
	m_rot(lin_algebra::identity_matrix<float>(3)),
	m_prev_time(get_curr_time())
{}

RotationIntegral::~RotationIntegral() {}


lin_algebra::matrix_t RotationIntegral::get_data() {
	typename VecGenerator<float,3>::vector_t data =
		DataFilter<typename VecGenerator<float,3>::vector_t, lin_algebra::matrix_t>::m_generator->get_data();

	// caclulate angles diff
	double m_rot_time = get_curr_time();
	double time_delta = m_rot_time - m_prev_time;
	m_prev_time = m_rot_time;

	float wx = -data[0]*time_delta / 180. * lin_algebra::PI;
	float wy = -data[1]*time_delta / 180. * lin_algebra::PI;
	float wz = -data[2]*time_delta / 180. * lin_algebra::PI;

	lin_algebra::matrix_t update(3, 3); // = W + I
	update(0,0) = 1.;		update(0,1) = -1.*wz;	update(0,2) = wy;
	update(1,0) = wz;   	update(1,1) = 1.;   	update(1,2) = -1.*wx;
	update(2,0) = -1.*wy;  	update(2,1) = wx;   	update(2,2) = 1.;

	// update the coordinate system
	m_rot = m_rot * update;

	// maintain the matrix ortho-normal
	lin_algebra::mat_col col0 = lin_algebra::mat_col(m_rot, 0);
	lin_algebra::mat_col col1 = lin_algebra::mat_col(m_rot, 1);
	lin_algebra::mat_col col2 = lin_algebra::mat_col(m_rot, 2);

	col2 = lin_algebra::cross_product(col0, col1);
	col0 = lin_algebra::cross_product(col1, col2);

	lin_algebra::normalize(col0);
	lin_algebra::normalize(col1);
	lin_algebra::normalize(col2);

	return  m_rot;
}


} // namespace filters
} // namespace stream

