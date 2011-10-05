#ifndef ACC_TO_EULER_FILTER_H_
#define ACC_TO_EULER_FILTER_H_

#include <boost/array.hpp>

#include <stream/util/lin_algebra.h>
#include <stream/generators.h>

namespace stream {
namespace filters {

class AccCompassRotation : public DataGenerator<lin_algebra::matrix_t> {
public:
	AccCompassRotation(VecGenerator<float,3> *acc, VecGenerator<float,3> *compass);

	lin_algebra::matrix_t get_data();

private:
	VecGenerator<float,3> *m_acc;
	VecGenerator<float,3> *m_compass;
};

} // namespace filters
} // namespace stream

#endif /* ACC_TO_EULER_FILTER_H_ */
