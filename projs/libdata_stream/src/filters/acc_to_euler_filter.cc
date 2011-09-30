#include "acc_to_euler_filter.h"
#include <cmath>
#include "stream/util/time.h"

namespace stream {
namespace filters {
AccToEulerFilter::AccToEulerFilter(VecGenerator<float,3>* data_gen):
	VecFilter<float,3>(data_gen)	{}


AccToEulerFilter::~AccToEulerFilter() {}

typename VecFilter<float,3>::vector_t AccToEulerFilter::get_data() {
	typename VecFilter<float,3>::vector_t data =
			VecFilter<float,3>::m_generator->get_data();
	lin_algebra::vector_t acc(3);
	acc[0] = data[0]; acc[1] = data[1]; acc[2] = data[2];
	lin_algebra::vector_t norm_acc(3);
	norm_acc = acc / lin_algebra::vec_len(acc);

	typename VecFilter<float, 3>::vector_t ans;

	// the pitch
	lin_algebra::vector_t z(3);
	z[0] = 0.; z[1] =  0.; z[2] =  1.;
	ans[0] = lin_algebra::angle_between(norm_acc, z);
	ans[1] = 0.;
	ans[2] = 0.;

	return  ans;
}

/*
typename VecFilter<float,3>::vector_t calc_euler_angles(){
	typename VecFilter<float,3>::vector_t ans;

	// The Pitch
	// Calculated by the calculating the angle between the plan's Z axis
	// and the Y axis.
	vector_t y(3); y[0] = 0.; y[1] = 1.; y[2] = 0.;
	ans[0] = 90. - angle_between(m_sum[2], y);

	// The Yaw
	// Calculate it by calculating the angle of the z projection on the xz
	// surface and the real world z.
	vector_t z_xz(3), world_z(3);
	z_xz[0] = m_sum[2][0] ;z_xz[1] = 0.; z_xz[2] = m_sum[2][2];
	world_z[0] = 0.; world_z[1] = 0.; world_z[2] = 1.;
	normalize(z_xz);
	ans[1] = angle_between(z_xz, world_z);
	if (m_sum[2][0] < 0) {
		ans[1] = 360-ans[1];
	}

	// The Roll
	// First, find the vector that is the crossection between the the plane
	// XY surface and the real world XZ surface. we call it rot_ref.
	float m = m_sum[0][1] / m_sum[1][1] * -1.;
	vector_t rot_ref = m_sum[0] + m*m_sum[1];
	normalize(rot_ref);

	// Roll, continues. Now find the angle between rot_ref and the plane's x
	// axis. In order to make the rotation be 360 degrees, we look at two things:
	//  - Is the plane up-side-down? this is calculated by checking if plane's y
	//    axis is pointing up or down: m_sum[1][1] < 0.
	//  - is the plan's X axis is above or below XZ surface? It is calculated
	//    by sign(m_sum[0][1]).
	ans[2] = -1.*sign(m_sum[0][1])*angle_between(rot_ref, m_sum[0]);
	if (m_sum[1][1] < 0.) {
		ans[2] = 180. - ans[2];
	}

	return ans;
}
*/

} // namespace filters
} // namespace stream
