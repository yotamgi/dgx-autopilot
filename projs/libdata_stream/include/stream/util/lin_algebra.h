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

void orthogonalize(vector_t& a, vector_t& b) {
	a[2] = -1.0 * (a[0]*b[0] + a[1]*b[1]) / b[2];
}

/**
 * Expects two normalized vectors
 */
inline float angle_between(const vector_t& a, const vector_t& b) {

	return std::acos(a[0]*b[0] + a[1]*b[1] + a[2]*b[2]) * 180. / PI;
}

inline float vec_len(const vector_t& vec) {
	return std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}

inline void normalize(vector_t& vec) {
	vec /= vec_len(vec);
}

}

#endif /* LIN_ALGEBRA_H_ */
