
inline vector_t::vector_t(float _x, float _y, float _z):
	x(_x), y(_y), z(_z)
{}

inline const vector_t& vector_t::operator += (const vector_t &other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

inline const vector_t& vector_t::operator -= (const vector_t &other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

inline const vector_t& vector_t::operator /= (const float& other) {
	x/=other;
	y/=other;
	z/=other;
	return *this;
}

inline const vector_t& vector_t::operator *= (const float& other) {
	x*=other;
	y*=other;
	z*=other;
	return *this;
}
