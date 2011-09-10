#ifndef EULER_ANGLES_INTEGRAL_H_
#define EULER_ANGLES_INTEGRAL_H_

#include <limits>
#include <sys/time.h>
#include <cmath>

#include <boost/array.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "generators.h"
#include "util/time.h"

/*
 * TODO:
 *  - seperate to inl
 */
template <typename T>
class EulerAnglesIntegral : public VecFilter<T,3> {
public:

	EulerAnglesIntegral(VecGenerator<T,3>* data_gen):
		VecFilter<T,3>(data_gen),
		m_sum_vec(3),
		m_prev_time(get_curr_time())
	{
		// initialize the vector to point right
		m_sum_vec[0] = 0.; m_sum_vec[1] =  0.; m_sum_vec[2] =  1.;

		// initialize the rotation to be zero
		m_sum_rot = 0.;
	}

	virtual ~EulerAnglesIntegral() {}

	typename VecFilter<T,3>::vector_t get_data() {
		typename VecFilter<T,3>::vector_t data =
				VecFilter<T,3>::m_generator->get_data();

		// integrate the data with time
		double curr_time = get_curr_time();
		double time_delta = curr_time - m_prev_time;
		m_prev_time = curr_time;

		// integrate the roll
		m_sum_rot += data[2] * time_delta;

		// rotate the vector
		T sx = std::sin(data[0] * time_delta / 180. * 3.141592);
		T cx = std::cos(data[0] * time_delta / 180. * 3.141592);
		T sy = std::sin(data[1] * time_delta / 180. * 3.141592);
		T cy = std::cos(data[1] * time_delta / 180. * 3.141592);

		boost::numeric::ublas::matrix<float> rot(3, 3);
		rot(0,0) = 1;	rot(0,1) = 0;	rot(0,2) = 0;
		rot(1,0) = 0;	rot(1,1) = cx;	rot(1,2) = sx*-1.;
		rot(2,0) = 0;	rot(2,1) = sx;	rot(2,2) = cx;

		m_sum_vec = boost::numeric::ublas::prod(rot, m_sum_vec);

		std::cout << m_sum_vec << std::endl;

		typename VecFilter<T,3>::vector_t ans;
		T vec_len = std::sqrt(m_sum_vec[0]*m_sum_vec[0] + m_sum_vec[1]*m_sum_vec[1] + m_sum_vec[2]*m_sum_vec[2]);
		ans[0] = std::acos( std::sqrt(m_sum_vec[2]*m_sum_vec[2] + m_sum_vec[0]*m_sum_vec[0]) / vec_len) * 180. / 3.141592;
		ans[1] = std::acos( std::sqrt(m_sum_vec[2]*m_sum_vec[2] + m_sum_vec[1]*m_sum_vec[1]) / vec_len) * 180. / 3.141592;
		ans[2] = m_sum_rot;

		std::cout << "angles are " << ans << std::endl;
		return  ans;
	}

private:
	/**
	 * In this integral, we save the vector pointing right and the rotation aroud it.
	 * The vector length should be 1.
	 */
	boost::numeric::ublas::vector<T> m_sum_vec;
	T m_sum_rot;

	double m_prev_time;
};

#endif /* EULER_ANGLES_INTEGRAL_H_ */
