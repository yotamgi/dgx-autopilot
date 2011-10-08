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
			VecGenerator<float,3> *compass);

	AccCompassRotation(
			VecGenerator<float,3> *acc,
			VecGenerator<float,3> *compass,
			VecGenerator<float,3>::vector_t expected_north);

	AccCompassRotation(
			VecGenerator<float,3> *acc,
			VecGenerator<float,3> *compass,
			float north_pitch_angle);

	lin_algebra::matrix_t get_data();

	DataGenerator<lin_algebra::matrix_t>* reliable_stream();

private:

	class AccCompassReliable : public DataGenerator<float> {
	public:
		AccCompassReliable(){}

		float get_data() { return reliability; }

	private:
		float reliability;
		friend class AccCompassRotation;
	};

	VecGenerator<float,3> *m_acc;
	VecGenerator<float,3> *m_compass;

	float m_north_pitch_angle;

	AccCompassReliable m_reliable_stream;
};

} // namespace filters
} // namespace stream

#endif /* ACC_TO_EULER_FILTER_H_ */
