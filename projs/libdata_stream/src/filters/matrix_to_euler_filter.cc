#include "matrix_to_euler_filter.h"

namespace stream {
namespace filters {

MatrixToEulerFilter::MatrixToEulerFilter(
		boost::shared_ptr<DataPopStream<lin_algebra::mat3f> > gen):
	StreamPopFilter<lin_algebra::mat3f, lin_algebra::vec3f>(gen)
{}

lin_algebra::vec3f MatrixToEulerFilter::get_data() {
	lin_algebra::mat3f rot_mat =  m_generator->get_data();
	lin_algebra::vec3f ans;

//	const lin_algebra::mat_col plan_x = lin_algebra::mat_col(rot_mat, 0);
//	const lin_algebra::mat_col plan_y = lin_algebra::mat_col(rot_mat, 1);
//	const lin_algebra::mat_col plan_z = lin_algebra::mat_col(rot_mat, 2);
//
//	// The Pitch
//	// Calculated by the calculating the angle between the plan's Z axis
//	// and the Y axis.
//	lin_algebra::vector_t y(3); y[0] = 0.; y[1] = 1.; y[2] = 0.;
//	ans[0] = 90. - lin_algebra::angle_between(plan_z, y);
//
//	// The Yaw
//	// Calculate it by calculating the angle of the z projection on the xz
//	// surface and the real world z.
//	lin_algebra::vector_t z_xz(3), world_z(3);
//	z_xz[0] = plan_z[0] ;z_xz[1] = 0.; z_xz[2] = plan_z[2];
//	world_z[0] = 0.; world_z[1] = 0.; world_z[2] = 1.;
//	lin_algebra::normalize(z_xz);
//	ans[1] = lin_algebra::angle_between(z_xz, world_z);
//	if (plan_z[0] < 0) {
//	    ans[1] = 360-ans[1];
//	}
//
//	// The Roll
//	// First, find the vector that is the crossection between the the plane
//	// XY surface and the real world XZ surface. we call it rot_ref.
//	float m = plan_x[1] / plan_y[1] * -1.;
//	lin_algebra::vector_t rot_ref = plan_x + m*plan_y;
//	lin_algebra::normalize(rot_ref);
//
//	// Roll, continues. Now find the angle between rot_ref and the plane's x
//	// axis. In order to make the rotation be 360 degrees, we look at two things:
//	//  - Is the plane up-side-down? this is calculated by checking if plane's y
//	//    axis is pointing up or down: m_sum[1][1] < 0.
//	//  - is the plan's X axis is above or below XZ surface? It is calculated
//	//    by sign(m_sum[0][1]).
//	ans[2] = -1.*lin_algebra::sign(plan_x[1])*lin_algebra::angle_between(rot_ref, plan_x);
//	if (plan_y[1] < 0.) {
//	    ans[2] = 180. - ans[2];
//	}

	ans[0] =  asin(rot_mat(1,2)) * 180. / lin_algebra::PI;
	ans[1] =  atan2(rot_mat(0,2), rot_mat(2,2)) * 180. / lin_algebra::PI;
	ans[2] = -atan2(rot_mat(1,0), rot_mat(1,1)) * 180. / lin_algebra::PI;

	return ans;
}



}  // namespace filters
}  // namespace stream

