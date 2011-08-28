#ifndef TYPES_H_
#define TYPES_H_

#include <boost/operators.hpp>

struct vector_t :
	boost::addable<vector_t>,
	boost::subtractable<vector_t>,
	boost::dividable2<vector_t, float>,
	boost::multipliable2<vector_t, float>
{
	vector_t(){}
	vector_t(float _x, float _y, float _z);

	const vector_t& operator += (const vector_t &other);
	const vector_t& operator -= (const vector_t &other);
	const vector_t& operator /= (const float& other);
	const vector_t& operator *= (const float& other);

	float x;
	float y;
	float z;
};

typedef vector_t angular_velocity_t;

#include "types.inl"

#endif /* TYPES_H_ */
