/*
 * lin_algebra.h
 *
 *  Created on: Sep 30, 2011
 *      Author: yotam
 */

#ifndef LIN_ALGEBRA_H_
#define LIN_ALGEBRA_H_

#include <armadillo>
#include <iterator>


namespace lin_algebra {

	using namespace arma;

	/////////////////////////////////////////////////////////////////////
	// Types
	//////////////////////////
	typedef arma::fvec::fixed<3>   	  vector_t;
	typedef arma::frowvec::fixed<3>   rowvec3f;

	typedef arma::fmat::fixed<3,3> 	  matrix_t;


	/////////////////////////////////////////////////////////////////////
	// Constants
	//////////////////////////
	const float PI = 3.14159265f;

	/////////////////////////////////////////////////////////////////////
	// Methods
	//////////////////////////
	//typedef boost::numeric::ublas::matrix_row<matrix_t> mat_row;
	//typedef boost::numeric::ublas::matrix_column<matrix_t> mat_col;

	template <typename Mat>
	Mat identity_matrix(size_t x, size_t y) {
		Mat m;
		m.eye(x,y);
		return m;
	}

	/**
	 * Expects two normalized vectors
	 */
	template <typename vec>
	inline float angle_between(const vec& a, const vec& b) {

		return std::acos(a[0]*b[0] + a[1]*b[1] + a[2]*b[2]) * 180. / PI;
	}

	template <typename Vector>
	inline float vec_len(const Vector& vec) {
		return std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
	}

	template <typename vec>
	inline vec normalize(const vec& v) {
		return v / vec_len(v);
	}

	inline float sign(float num) {
		return (num>0.)?1.:-1.;
	}

	template <typename Vector>
	inline Vector cross_product(Vector a, Vector b) {
		Vector ans;
		ans[0] = a[1]*b[2] - a[2]*b[1];
		ans[1] = a[2]*b[0] - a[0]*b[2];
		ans[2] = a[0]*b[1] - a[1]*b[0];
		return ans;
	}

} // namepsace lin_algebra

/**
 * Global vector operators
 */
inline std::ostream& operator << (std::ostream& os, const lin_algebra::vector_t &v) {
	os << '[';
	std::copy(v.begin(), v.end()-1, std::ostream_iterator<lin_algebra::vector_t::value_type>(os, ", "));
	os << v[2] << "]";
	return os;
}

inline std::istream& operator >> (std::istream &is, lin_algebra::vector_t &v) {

    char ch;
    if (is >> ch && ch != '[') {
        is.putback (ch);
        is.setstate (std::ios_base::failbit);
    };
    for (size_t i=0; i<2; i++) {
    	is >> v[i] >> ch;
    	if (ch != ',') {
    		is.putback (ch);
    		is.setstate (std::ios_base::failbit);
    		break;
    	}
    }
	is >> v[2] >> ch;
	if (ch != ']') {
		is.putback (ch);
		is.setstate (std::ios_base::failbit);
	}

    return is;
}



#endif /* LIN_ALGEBRA_H_ */
