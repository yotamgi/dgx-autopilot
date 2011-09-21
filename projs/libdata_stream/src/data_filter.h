
#ifndef DATA_FILTER_H_
#define DATA_FILTER_H_

#include "generators.h"

namespace stream {

/**
 * A filter, which is a decoretor to the DataGenerator class.
 * To implement it, one must implement the get_data function.
 */
template <typename data_t>
class DataFilter : public DataGenerator<data_t> {
public:

	/**
	 * Inits the filter with the DataGenerator object it will filter
	 */
	DataFilter(DataGenerator<data_t>* generator):m_generator(generator) {}

	/**
	 * passes the question to the generator, because there is no way how data can
	 * be lossed in a filter.
	 */
	bool is_data_losed() { return m_generator->is_data_losed(); }

protected:

	/**
	 * The generator that it filters.
	 */
	DataGenerator<data_t>* m_generator;
};


/**
 * Specific filter for VecGenerators.
 */
template <typename T, size_t N>
class VecFilter : public VecGenerator< T,N >
{
public:
	typedef typename VecGenerator< T,N >::vector_t vector_t;

	VecFilter(VecGenerator<T,N>* generator):m_generator(generator) {}

	bool is_data_losed() { return m_generator->is_data_losed(); }

protected:

	VecGenerator<T,N>* m_generator;
};

} // namespace stream

#endif /* DATA_FILTER_H_ */
