
#ifndef DATA_FILTER_H_
#define DATA_FILTER_H_

#include "generators.h"
#include "collector.h"
#include <boost/shared_ptr.hpp>

namespace stream {

/**
 * A filter, which is a decoretor to the DataGenerator class.
 * To implement it, one must implement the get_data function.
 */
template <typename data_source_t, typename data_t = data_source_t>
class DataGenFilter : public DataGenerator<data_t> {
public:

	/**
	 * Inits the filter with the DataGenerator object it will filter
	 */
	DataGenFilter(boost::shared_ptr<DataGenerator<data_source_t> > generator):m_generator(generator) {}

protected:

	/**
	 * The generator that it filters.
	 */
	boost::shared_ptr<DataGenerator<data_source_t> > m_generator;
};


/**
 * A filter, which is a decoretor to the DataGenerator class.
 * To implement it, one must implement the get_data function.
 */
template <typename data_source_t, typename data_t = data_source_t>
class DataColFilter : public DataCollector<data_t> {
public:

	/**
	 * Inits the filter with the DataGenerator object it will filter
	 */
	DataColFilter(boost::shared_ptr<DataCollector<data_source_t> > collector):m_collector(collector) {}

protected:

	/**
	 * The generator that it filters.
	 */
	boost::shared_ptr<DataGenerator<data_source_t> > m_collector;
};

/**
 * The Generic filter class - for any kind of stream
 */
template <typename data_source_t, typename data_t = data_source_t>
class DataFilter : public DataGenFilter<data_t>, public DataCollector<data_t> {};



} // namespace stream

#endif /* DATA_FILTER_H_ */
