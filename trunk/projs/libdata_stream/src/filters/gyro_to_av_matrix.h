#ifndef EULER_ANGLES_INTEGRAL_H_
#define EULER_ANGLES_INTEGRAL_H_

#include "stream/generators.h"
#include "stream/data_filter.h"
#include "stream/util/lin_algebra.h"

namespace stream {
namespace filters {


class GyroToAVMatrix :
	public DataFilter<lin_algebra::vector_t, lin_algebra::matrix_t>
{
public:

	GyroToAVMatrix(boost::shared_ptr<DataGenerator<lin_algebra::vector_t> > data_gen);
	virtual ~GyroToAVMatrix();

	lin_algebra::matrix_t get_data();
};

} // namespace filters
} // namespace stream

#endif /* EULER_ANGLES_INTEGRAL_H_ */
