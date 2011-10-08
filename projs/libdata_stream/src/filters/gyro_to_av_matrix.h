#ifndef EULER_ANGLES_INTEGRAL_H_
#define EULER_ANGLES_INTEGRAL_H_

#include "stream/generators.h"
#include "stream/data_filter.h"
#include "stream/util/lin_algebra.h"

namespace stream {
namespace filters {


class GyroToAVMatrix :
	public DataFilter<typename VecGenerator<float,3>::vector_t, lin_algebra::matrix_t>
{
public:


	GyroToAVMatrix(VecGenerator<float,3>* data_gen);
	virtual ~GyroToAVMatrix();

	lin_algebra::matrix_t get_data();
};

} // namespace filters
} // namespace stream

#endif /* EULER_ANGLES_INTEGRAL_H_ */
