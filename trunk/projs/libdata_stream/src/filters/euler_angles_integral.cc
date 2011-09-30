
#include "euler_angles_integral.h"
#include <cmath>
#include "stream/util/time.h"

namespace stream {
namespace filters {

EulerAnglesIntegral::EulerAnglesIntegral(VecGenerator<float,3>* data_gen):
	VecFilter<float,3>(data_gen),
	m_rot(lin_algebra::identity_matrix<float>(3)),
	m_prev_time(get_curr_time())
{}

EulerAnglesIntegral::~EulerAnglesIntegral() {}


typename VecFilter<float,3>::vector_t EulerAnglesIntegral::get_data() {
	typename VecFilter<float,3>::vector_t data =
			VecFilter<float,3>::m_generator->get_data();

	// caclulate angles diff
	double m_rot_time = get_curr_time();
	double time_delta = m_rot_time - m_prev_time;
	m_prev_time = m_rot_time;

	float wx = data[0]*time_delta / 180. * lin_algebra::PI;
	float wy = data[1]*time_delta / 180. * lin_algebra::PI;
	float wz = data[2]*time_delta / 180. * lin_algebra::PI;

	lin_algebra::matrix_t update(3, 3);
	update(0,0) = 0.;		update(0,1) = -1.*wz;	update(0,2) = wy;
	update(1,0) = wz;   	update(1,1) = 0.;   	update(1,2) = -1.*wx;
	update(2,0) = -1.*wy;  	update(2,1) = wx;   	update(2,2) = 0.;

	// update the coordinate system
	m_rot = m_rot +  lin_algebra::prod(update, m_rot);

	// maintain the matrix ortho-normal
	lin_algebra::mat_row row0 = lin_algebra::mat_row(m_rot, 0);
	lin_algebra::mat_row row1 = lin_algebra::mat_row(m_rot, 1);
	lin_algebra::mat_row row2 = lin_algebra::mat_row(m_rot, 2);

	lin_algebra::orthogonalize(row0, row1);
	lin_algebra::orthogonalize(row1, row2);
	lin_algebra::orthogonalize(row0, row2);

	lin_algebra::normalize(row0);
	lin_algebra::normalize(row1);
	lin_algebra::normalize(row2);

	typename VecFilter<float,3>::vector_t ans = calc_euler_angles();
	std::cout << "angle is " << ans << std::endl;
	return  ans;
}

typename VecFilter<float,3>::vector_t EulerAnglesIntegral::calc_euler_angles(){
	typename VecFilter<float,3>::vector_t ans;

	const lin_algebra::mat_row plan_x = lin_algebra::mat_row(m_rot, 0);
	const lin_algebra::mat_row plan_y = lin_algebra::mat_row(m_rot, 1);
	const lin_algebra::mat_row plan_z = lin_algebra::mat_row(m_rot, 2);

	// The Pitch
	// Calculated by the calculating the angle between the plan's Z axis
	// and the Y axis.
	lin_algebra::vector_t y(3); y[0] = 0.; y[1] = 1.; y[2] = 0.;
	ans[0] = 90. - lin_algebra::angle_between(plan_z, y);

	// The Yaw
	// Calculate it by calculating the angle of the z projection on the xz
	// surface and the real world z.
	lin_algebra::vector_t z_xz(3), world_z(3);
	z_xz[0] = plan_z[0] ;z_xz[1] = 0.; z_xz[2] = plan_z[2];
	world_z[0] = 0.; world_z[1] = 0.; world_z[2] = 1.;
	lin_algebra::normalize(z_xz);
	ans[1] = lin_algebra::angle_between(z_xz, world_z);
	if (plan_z[0] < 0) {
		ans[1] = 360-ans[1];
	}

	// The Roll
	// First, find the vector that is the crossection between the the plane
	// XY surface and the real world XZ surface. we call it rot_ref.
	float m = plan_x[1] / plan_y[1] * -1.;
	lin_algebra::vector_t rot_ref = plan_x + m*plan_y;
	lin_algebra::normalize(rot_ref);

	// Roll, continues. Now find the angle between rot_ref and the plane's x
	// axis. In order to make the rotation be 360 degrees, we look at two things:
	//  - Is the plane up-side-down? this is calculated by checking if plane's y
	//    axis is pointing up or down: m_sum[1][1] < 0.
	//  - is the plan's X axis is above or below XZ surface? It is calculated
	//    by sign(m_sum[0][1]).
	ans[2] = -1.*sign(plan_x[1])*lin_algebra::angle_between(rot_ref, plan_x);
	if (plan_y[1] < 0.) {
		ans[2] = 180. - ans[2];
	}

//		ans[0] = 0.;//atan2(m_sum[2][1],m_sum[1][1]) * 180. / PI;
//		ans[1] = atan2(m_sum[0][0], m_sum[0][2]) * 180. / PI;
//		ans[2] = asin(m_sum[0][1]) * 180. / PI;
	return ans;
}

float EulerAnglesIntegral::sign(float num) {
	return (num>0.)?1.:-1.;
}


} // namespace filters
} // namespace stream

