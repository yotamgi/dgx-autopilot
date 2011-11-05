/*
 * matrix_to_euler_filter.h
 *
 *  Created on: Oct 5, 2011
 *      Author: yotam
 */

#ifndef MATRIX_TO_EULER_FILTER_H_
#define MATRIX_TO_EULER_FILTER_H_

#include <stream/data_filter.h>
#include <stream/util/lin_algebra.h>

namespace stream {
namespace filters {

class MatrixToEulerFilter :
	public DataFilter<lin_algebra::matrix_t, lin_algebra::vector_t>
{
public:
	MatrixToEulerFilter(boost::shared_ptr<DataGenerator<lin_algebra::matrix_t> > gen);
	virtual ~MatrixToEulerFilter() {}

	lin_algebra::vector_t get_data();
};



}  // namespace filters
}  // namespace stream

#endif /* MATRIX_TO_EULER_FILTER_H_ */
