#ifndef EULER_ANGLES_INTEGRAL_H_
#define EULER_ANGLES_INTEGRAL_H_

#include <limits>
#include <sys/time.h>
#include <cmath>

#include <boost/array.hpp>
#include "stream/generators.h"
#include "stream/data_filter.h"
#include "stream/util/time.h"
#include "stream/util/lin_algebra.h"

namespace stream {
namespace filters {

using namespace lin_algebra;

/*
 * TODO:
 *  - seperate to inl
 */
template <typename float_t>
class EulerAnglesIntegral : public VecFilter<float_t,3> {
public:


	EulerAnglesIntegral(VecGenerator<float_t,3>* data_gen):
		VecFilter<float_t,3>(data_gen),
		m_sum((vector_t[3]){ vector_t(3), vector_t(3),vector_t(3) }	),
		m_prev_time(get_curr_time())
	{
		// initialize the coordinate system
		m_sum[0][0] = 1.; m_sum[0][1] =  0.; m_sum[0][2] =  0.;
		m_sum[1][0] = 0.; m_sum[1][1] =  1.; m_sum[1][2] =  0.;
		m_sum[2][0] = 0.; m_sum[2][1] =  0.; m_sum[2][2] =  1.;
	}


	virtual ~EulerAnglesIntegral() {}

	typename VecFilter<float_t,3>::vector_t get_data() {
		typename VecFilter<float_t,3>::vector_t data =
				VecFilter<float_t,3>::m_generator->get_data();

		// caclulate angles diff
		double curr_time = get_curr_time();
		double time_delta = curr_time - m_prev_time;
		m_prev_time = curr_time;

		float_t wx = data[0]*time_delta / 180. * PI;
		float_t wy = data[1]*time_delta / 180. * PI;
		float_t wz = data[2]*time_delta / 180. * PI;

		matrix_t update(3, 3);
		update(0,0) = 0.;		update(0,1) = -1.*wz;	update(0,2) = wy;
		update(1,0) = wz;   	update(1,1) = 0.;   	update(1,2) = -1.*wx;
		update(2,0) = -1.*wy;  	update(2,1) = wx;   	update(2,2) = 0.;

		matrix_t curr(3, 3);
		curr(0,0) = m_sum[0][0];	curr(0,1) = m_sum[0][1];	curr(0,2) = m_sum[0][2];
		curr(1,0) = m_sum[1][0];   	curr(1,1) = m_sum[1][1];   	curr(1,2) = m_sum[1][2];
		curr(2,0) = m_sum[2][0];  	curr(2,1) = m_sum[2][1];   	curr(2,2) = m_sum[2][2];

		curr = curr +  boost::numeric::ublas::prod(update, curr);

//		m_sum[0][0] = curr(0,0);	m_sum[0][1] = curr(0,1);	m_sum[0][2] = curr(0,2);
//		m_sum[1][0] = curr(1,0);   	m_sum[1][1] = curr(1,1);   	m_sum[1][2] = curr(1,2);
//		m_sum[2][0] = curr(2,0);  	m_sum[2][1] = curr(2,1);   	m_sum[2][2] = curr(2,2);
		m_sum[0] = mat_row(curr, 0);
		m_sum[1] = mat_row(curr, 1);
		m_sum[2] = mat_row(curr, 2);

		// maintain the coordinate orthogonal
		orthogonalize(m_sum[0], m_sum[1]);
		orthogonalize(m_sum[1], m_sum[2]);
		orthogonalize(m_sum[0], m_sum[2]);

		normalize(m_sum[0]);
		normalize(m_sum[1]);
		normalize(m_sum[2]);

		typename VecFilter<float_t,3>::vector_t ans = calc_euler_angles();
		std::cout << "angle is " << ans << std::endl;
		return  ans;
	}

private:

	typename VecFilter<float_t,3>::vector_t calc_euler_angles(){
		typename VecFilter<float_t,3>::vector_t ans;

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

//		ans[0] = 0.;//atan2(m_sum[2][1],m_sum[1][1]) * 180. / PI;
//		ans[1] = atan2(m_sum[0][0], m_sum[0][2]) * 180. / PI;
//		ans[2] = asin(m_sum[0][1]) * 180. / PI;
		return ans;
	}

	float_t sign(float_t num) {
		return (num>0.)?1.:-1.;
	}

	/**
	 * In this integral, we rotate an coordinate system according to the
	 * angles diff it gets.
	 * float_those are the three vectors that represents the coordinate system
	 * Index 0 is the X vector, 1 is the Y vector and 2 is Z.
	 */
	vector_t m_sum[3];

	/**
	 * Needed for time_delta calculation.
	 */
	double m_prev_time;
};

} // namespace filters
} // namespace stream

#endif /* EULER_ANGLES_INTEGRAL_H_ */
