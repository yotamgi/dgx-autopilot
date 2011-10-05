/*
 * matrix_to_euler_filter.h
 *
 *  Created on: Oct 5, 2011
 *      Author: yotam
 */

#ifndef MATRIX_TO_EULER_FILTER_H_
#define MATRIX_TO_EULER_FILTER_H_

#include <data_filter.h>
#include <util/lin_algebra.h>

namespace stream {
namespace filters {

class MatrixToEulerFilter :
	public DataFilter<lin_algebra::matrix_t, typename VecGenerator<float,3>::vector_t>
{
public:
	MatrixToEulerFilter(DataGenerator<lin_algebra::matrix_t>* gen);

	typename VecGenerator<float,3>::vector_t get_data();
};



}  // namespace filters
}  // namespace stream

#endif /* MATRIX_TO_EULER_FILTER_H_ */
