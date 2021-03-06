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
	typedef arma::fvec::fixed<3>   	  vec3f;
	typedef arma::frowvec::fixed<3>   rowvec3f;
	typedef arma::fmat::fixed<3,3> 	  mat3f;
	typedef arma::fvec::fixed<2>   	  vec2f;
	typedef arma::frowvec::fixed<2>   rowvec2f;


	/////////////////////////////////////////////////////////////////////
	// Constants
	//////////////////////////
	const float PI = M_PI;

	/////////////////////////////////////////////////////////////////////
	// Methods
	//////////////////////////

	inline vec3f create_vec3f(float a, float b, float c) {
		vec3f ans;
		ans[0] = a;
		ans[1] = b;
		ans[2] = c;
		return ans;
	}

	inline vec2f create_vec2f(float a, float b) {
		vec2f ans;
		ans[0] = a;
		ans[1] = b;
		return ans;
	}


	template <size_t I>
	float get(vec3f v) {
		return v[I];
	}
	template <size_t I1, size_t I2>
	vec2f get(vec3f v) {
		vec2f ans;
		ans[0] = v[I1];
		ans[1] = v[I2];
		return ans;
	}

	template <typename Mat>
	Mat identity_matrix(size_t x, size_t y) {
		Mat m;
		m.eye(x,y);
		return m;
	}

	/**
	 * Retunrs random float between -0.5 to 0.5
	 */
	inline float frand() {
		return (float)rand()/(float)RAND_MAX - 0.5;
	}

	/**
	 * Expects two normalized vectors
	 */
	template <typename Vec3>
	inline float angle_between(const Vec3& a, const Vec3& b) {

		return std::acos(a[0]*b[0] + a[1]*b[1] + a[2]*b[2]) * 180. / PI;
	}

	template <typename Vec3>
	inline float vec_len(const Vec3& vec) {
		return std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
	}

	template <typename Vec>
	inline Vec normalize(const Vec& v) {
		return v / vec_len(v);
	}

	inline float sign(float num) {
		return (num>0.)?1.:-1.;
	}

	template <typename Vec3>
	inline Vec3 cross_product(Vec3 a, Vec3 b) {
		Vec3 ans;
		ans[0] = a[1]*b[2] - a[2]*b[1];
		ans[1] = a[2]*b[0] - a[0]*b[2];
		ans[2] = a[0]*b[1] - a[1]*b[0];
		return ans;
	}

} // namepsace lin_algebra

/**
 * Global vector operators
 */
inline std::ostream& operator << (std::ostream& os, const lin_algebra::vec3f &v) {
	v.save(os, arma::arma_ascii);
	return os;
}
inline std::istream& operator >> (std::istream &is, lin_algebra::vec3f &v) {
	v.load(is, arma::arma_ascii);
    return is;
}
inline std::ostream& operator << (std::ostream& os, const lin_algebra::vec2f &v) {
	v.save(os, arma::arma_ascii);
	return os;
}
inline std::istream& operator >> (std::istream &is, lin_algebra::vec2f &v) {
	v.load(is, arma::arma_ascii);
    return is;
}



#endif /* LIN_ALGEBRA_H_ */
