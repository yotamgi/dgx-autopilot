/*
 * lin_algebra.h
 *
 *  Created on: Sep 30, 2011
 *      Author: yotam
 */

#ifndef LIN_ALGEBRA_H_
#define LIN_ALGEBRA_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>


namespace lin_algebra {

using namespace boost::numeric::ublas;

/////////////////////////////////////////////////////////////////////
// Types
//////////////////////////
typedef boost::numeric::ublas::vector<float> vector_t;
typedef boost::numeric::ublas::matrix<float> matrix_t;

/////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////
const float PI = 3.14159265;


/////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////
typedef boost::numeric::ublas::matrix_row<matrix_t> mat_row;
typedef boost::numeric::ublas::matrix_column<matrix_t> mat_col;

/**
 * Expects two normalized vectors
 */
inline float angle_between(const vector_t& a, const vector_t& b) {

	return std::acos(a[0]*b[0] + a[1]*b[1] + a[2]*b[2]) * 180. / PI;
}

template <typename Vector>
inline float vec_len(const Vector& vec) {
	return std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}

template <typename vec>
inline void normalize(vec& v) {
	v /= vec_len(v);
}

inline float sign(float num) {
	return (num>0.)?1.:-1.;
}

inline vector_t cross_product(vector_t a, vector_t b) {
	vector_t ans(3);
	ans[0] = a[1]*b[2] - a[2]*b[1];
	ans[1] = a[2]*b[0] - a[0]*b[2];
	ans[2] = a[0]*b[1] - a[1]*b[0];
	return ans;
}

} // namepsace lin_algebra

inline lin_algebra::vector_t operator * (lin_algebra::matrix_t M, lin_algebra::vector_t v) {
	return lin_algebra::prod(M, v);
}

inline lin_algebra::vector_t operator * (lin_algebra::vector_t v, lin_algebra::matrix_t M) {
	return lin_algebra::prod(v, M);
}

inline lin_algebra::matrix_t operator * (lin_algebra::matrix_t M1, lin_algebra::matrix_t M2) {
	return lin_algebra::prod(M1, M2);
}


#endif /* LIN_ALGEBRA_H_ */
