#ifndef EULER_ANGLES_INTEGRAL_H_
#define EULER_ANGLES_INTEGRAL_H_

#include "stream/generators.h"
#include "stream/data_filter.h"
#include "stream/util/lin_algebra.h"

namespace stream {
namespace filters {


class EulerAnglesIntegral : public VecFilter<float,3> {
public:


	EulerAnglesIntegral(VecGenerator<float,3>* data_gen);
	virtual ~EulerAnglesIntegral();

	typename VecFilter<float,3>::vector_t get_data();

private:

	typename VecFilter<float,3>::vector_t calc_euler_angles();

	float sign(float num);

	/**
	 * In this integral, we rotate an coordinate system according to the
	 * angles diff it gets.
	 * floathose are the three vectors that represents the coordinate system
	 * Index 0 is the X vector, 1 is the Y vector and 2 is Z.
	 */
	lin_algebra::matrix_t m_rot;

	/**
	 * Needed for time_delta calculation.
	 */
	double m_prev_time;
};

} // namespace filters
} // namespace stream

#endif /* EULER_ANGLES_INTEGRAL_H_ */
