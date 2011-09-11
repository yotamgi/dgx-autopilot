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
template <typename float_t>
class EulerAnglesIntegral : public VecFilter<float_t,3> {
public:

	typedef typename boost::numeric::ublas::vector<float_t> vector_t;

	EulerAnglesIntegral(VecGenerator<float_t,3>* data_gen):
		VecFilter<float_t,3>(data_gen),
		m_sum((vector_t[3]){ vector_t(3), vector_t(3),vector_t(3) }	),
		rot_sum(0.),
		m_prev_time(get_curr_time()),
		PI(3.141592654)
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

		rot_sum += data[2] * time_delta;

		// rotate the coordinate system
		rotate_coordinate_around(0, data[0]*time_delta);
		rotate_coordinate_around(1, data[1]*time_delta);
		rotate_coordinate_around(2, data[2]*time_delta);

		// calculate the euler angle
		typename VecFilter<float_t,3>::vector_t ans;
		float_t z_len = vec_len(m_sum[2]);
		ans[0] = sign(m_sum[2][1]) * std::acos(std::sqrt(m_sum[2][2]*m_sum[2][2] + m_sum[2][0]*m_sum[2][0]) / z_len) * 180. / PI;
		ans[1] = sign(m_sum[2][0]) * std::acos(std::sqrt(m_sum[2][2]*m_sum[2][2] + m_sum[2][1]*m_sum[2][1]) / z_len) * 180. / PI;
		ans[2] = rot_sum;

		std::cout << "Z is " << m_sum[2] << std::endl;
		return  ans;
	}

private:

	void rotate_coordinate_around(size_t index, float howmuch) {

		vector_t& a = m_sum[(index+1) % 3];
		vector_t& b = m_sum[(index+2) % 3];

		float_t rot_factor = std::tan(howmuch / 180. * PI);
		vector_t tmp_a = a - rot_factor * b;
		b = b + rot_factor * a;
		a = tmp_a;

		normalize(a);
		normalize(b);
	}

	void normalize(vector_t& vec) {
		vec /= vec_len(vec);
	}

	float_t sign(float_t num) {
		return (num>0.)?1.:-1.;
	}

	float_t vec_len(const vector_t& vec) const {
		return std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
	}

	/**
	 * In this integral, we rotate an coordinate system according to the
	 * angles diff it gets.
	 * float_those are the three vectors that represents the coordinate system
	 * Index 0 is the X vector, 1 is the Y vector and 2 is Z.
	 */
	vector_t m_sum[3];

	float_t rot_sum;

	/**
	 * Needed for time_delta calculation.
	 */
	double m_prev_time;

	const float_t PI;
};


#endif /* EULER_ANGLES_INTEGRAL_H_ */
