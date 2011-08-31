#ifndef GANERATOR_H_
#define GANERATOR_H_

#include <boost/array.hpp>

/**
 * Represents a any type of data generator. It may be a sensor, or a combination
 * of some.
 */
template <typename data_t>
class DataGenerator {
public:

	virtual ~DataGenerator() {}

	/**
	 * Suppose to return the current value/sample of the data.
	 */
	virtual data_t get_data() = 0;

	/**
	 * Returns true if there was data losed in the generator due to insufficient
	 * reading speed.
	 * Should turn false after reading from it.
	 */
	virtual bool is_data_losed() = 0;

};

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
 * Specific Vector type Data Generator.
 * It defines the typedef vector_t which is the type of the vector it generates.
 */
template <typename T, size_t N>
class VecGenerator : public DataGenerator< boost::array<T,N> >
{
public:
	typedef boost::array<T,N> vector_t;
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


#endif /* GANERATOR_H_ */
