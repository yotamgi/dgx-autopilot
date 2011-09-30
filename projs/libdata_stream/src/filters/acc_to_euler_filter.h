#ifndef ACC_TO_EULER_FILTER_H_
#define ACC_TO_EULER_FILTER_H_

#include <boost/array.hpp>

#include "stream/util/lin_algebra.h"
#include "stream/data_filter.h"

namespace stream {
namespace filters {

/*
 * TODO:
 *  - seperate to inl
 */
class AccToEulerFilter : public VecFilter<float,3> {
public:

	AccToEulerFilter(VecGenerator<float,3>* data_gen);

	virtual ~AccToEulerFilter();

	typename VecFilter<float,3>::vector_t get_data();
};

} // namespace filters
} // namespace stream

#endif /* ACC_TO_EULER_FILTER_H_ */
