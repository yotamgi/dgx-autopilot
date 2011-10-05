
#ifndef DATA_FILTER_H_
#define DATA_FILTER_H_

#include "generators.h"

namespace stream {

/**
 * A filter, which is a decoretor to the DataGenerator class.
 * To implement it, one must implement the get_data function.
 */
template <typename data_t, typename data_source_t = data_t>
class DataFilter : public DataGenerator<data_t> {
public:

	/**
	 * Inits the filter with the DataGenerator object it will filter
	 */
	DataFilter(DataGenerator<data_source_t>* generator):m_generator(generator) {}

protected:

	/**
	 * The generator that it filters.
	 */
	DataGenerator<data_source_t>* m_generator;
};

/**
 * Specific filter for VecGenerators.
 */
template <typename T, size_t N, typename SourceT=T, size_t SourceN=N>
class VecFilter : public VecGenerator< T,N >
{
public:
	typedef typename VecGenerator< T,N >::vector_t vector_t;

	VecFilter(VecGenerator<SourceT,SourceN>* generator):m_generator(generator) {}

protected:

	VecGenerator<SourceT,SourceN>* m_generator;
};

} // namespace stream

#endif /* DATA_FILTER_H_ */
