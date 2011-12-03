/*
 * watch_filter.h
 *
 *  Created on: Nov 7, 2011
 *      Author: yotam
 */

#ifndef WATCH_FILTER_H_
#define WATCH_FILTER_H_

#include <stream/data_filter.h>

namespace stream {
namespace filters {

/**
 * The Watch Filter.
 * This filter allows you to "look" at a stream transportation without actually applying the stream
 * generator - just by checking what was the last thing that passed in the stream
 * This is done by the get_watch_stream func that returns generators that only "look" and does not
 * ask for new data.
 * Apart for that, this class is a normal bypass filter.
 */
template <class T>
class WatchFilter : public StreamFilter<T> {
public:
	WatchFilter(boost::shared_ptr< DataPopStream<T> > gen):
		StreamFilter<T>(gen),
		m_watch_stream(new WatchStream(this))
	{}
	WatchFilter(boost::shared_ptr< DataPushStream<T> > col):
		StreamFilter<T>(col),
		m_watch_stream(new WatchStream(this))
	{}
	~WatchFilter() {}

	/**
	 * The normal bypass filter get_data function.
	 */
	T get_data() {
		m_watched = StreamPopFilter<T>::m_generator->get_data();
		return m_watched;
	}

	void set_data(const T& data) {
		m_watched = data;
		StreamPushFilter<T>::m_collector->set_data(data);
	}


	/**
	 * Returns the watch stream - the actuall stream that will show you the strean data without
	 * generating new data in the original stream.
	 */
	boost::shared_ptr< DataPopStream<T> > get_watch_stream() const {
		return m_watch_stream;
	}

private:

	/**
	 * Watch Stream.
	 * A dummy stream that gets a WatchFilter and returns allways its m_watched data.
	 * returned by WatchFilter::get_watch_stream method.s
	 */
	class WatchStream : public DataPopStream<T> {
	public:
		WatchStream(WatchFilter* father):m_father(father){}

		T get_data() {
			return m_father->m_watched;
		}

	private:
		WatchFilter<T>* m_father;
	};

	/** The last data that passed in the stream */
	T m_watched;

	/** the watch stream - the stream that is returned by get_watch_stream */
	boost::shared_ptr< DataPopStream<T> > m_watch_stream;
};


}  // namespace filters
}  // namespace stream

#endif /* WATCH_FILTER_H_ */
