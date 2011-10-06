#ifndef ACC_TO_EULER_FILTER_H_
#define ACC_TO_EULER_FILTER_H_

#include <boost/array.hpp>

#include <stream/util/lin_algebra.h>
#include <stream/generators.h>

namespace stream {
namespace filters {

class AccCompassRotation : public DataGenerator<lin_algebra::matrix_t> {
public:
	AccCompassRotation(
			VecGenerator<float,3> *acc,
			VecGenerator<float,3> *compass,
			VecGenerator<float,3>::vector_t expected_north);

	AccCompassRotation(
			VecGenerator<float,3> *acc,
			VecGenerator<float,3> *compass,
			float north_pitch_angle,
			float north_yaw_angle);

	lin_algebra::matrix_t get_data();

private:
	VecGenerator<float,3> *m_acc;
	VecGenerator<float,3> *m_compass;

	float m_north_pitch_angle;
	float m_north_yaw_angle;
};

} // namespace filters
} // namespace stream

#endif /* ACC_TO_EULER_FILTER_H_ */
