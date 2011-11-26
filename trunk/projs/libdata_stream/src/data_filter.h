
#ifndef DATA_FILTER_H_
#define DATA_FILTER_H_

#include "data_pop_stream.h"
#include "collector.h"
#include <boost/shared_ptr.hpp>

namespace stream {

/**
 * A filter, which is a decoretor to the DataPopStream class.
 * To implement it, one must implement the get_data function.
 */
template <typename data_source_t, typename data_t = data_source_t>
class StreamPopFilter : public DataPopStream<data_t> {
public:

	/**
	 * Inits the filter with the DataPopStream object it will filter
	 */
	StreamPopFilter(boost::shared_ptr<DataPopStream<data_source_t> > generator):m_generator(generator) {}

protected:

	/**
	 * The generator that it filters.
	 */
	boost::shared_ptr<DataPopStream<data_source_t> > m_generator;
};


/**
 * A filter, which is a decoretor to the DataPopStream class.
 * To implement it, one must implement the get_data function.
 */
template <typename data_source_t, typename data_t = data_source_t>
class StreamPushFilter : public DataPushStream<data_t> {
public:

	/**
	 * Inits the filter with the DataPopStream object it will filter
	 */
	StreamPushFilter(boost::shared_ptr<DataPushStream<data_source_t> > collector):m_collector(collector) {}

protected:

	/**
	 * The generator that it filters.
	 */
	boost::shared_ptr<DataPushStream<data_source_t> > m_collector;
};

/**
 * The Generic filter class - for any kind of stream
 */
template <typename data_source_t, typename data_t = data_source_t>
class StreamFilter : 	public StreamPopFilter<data_t>,
						public StreamPushFilter<data_t>
{
public:
	StreamFilter(boost::shared_ptr<DataPopStream<data_source_t> > generator,
				boost::shared_ptr<DataPushStream<data_source_t> > collector):
		StreamPopFilter<data_t>(generator),
		StreamPushFilter<data_t>(collector)
	{}

	StreamFilter(boost::shared_ptr<DataPushStream<data_source_t> > collector):
		StreamPopFilter<data_t>(boost::shared_ptr<DataPopStream<data_source_t> >()),
		StreamPushFilter<data_t>(collector)
	{}

	StreamFilter(boost::shared_ptr<DataPopStream<data_source_t> > generator):
		StreamPopFilter<data_t>(generator),
		StreamPushFilter<data_t>(boost::shared_ptr<DataPushStream<data_source_t> >())
	{}


};



} // namespace stream

#endif /* DATA_FILTER_H_ */
