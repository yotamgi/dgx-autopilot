#ifndef EULER_ANGLES_INTEGRAL_H_
#define EULER_ANGLES_INTEGRAL_H_

#include "stream/data_pop_stream.h"
#include "stream/data_filter.h"
#include "stream/util/lin_algebra.h"

namespace stream {
namespace filters {


class GyroToAVMatrix :
	public StreamPopFilter<lin_algebra::vec3f, lin_algebra::mat3f>
{
public:

	GyroToAVMatrix(boost::shared_ptr<DataPopStream<lin_algebra::vec3f> > data_gen);
	virtual ~GyroToAVMatrix();

	lin_algebra::mat3f get_data();
};

} // namespace filters
} // namespace stream

#endif /* EULER_ANGLES_INTEGRAL_H_ */
