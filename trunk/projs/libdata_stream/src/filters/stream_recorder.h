#ifndef STREAM_RECORDER_H_
#define STREAM_RECORDER_H_

#include <stream/func_filter.h>
#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <stream/util/time.h>

#include <boost/shared_ptr.hpp>
#include <ostream>
#include <string>
#include <stdexcept>

namespace stream {
namespace filters {

/**
 * Recorder Functor.
 * This is a functor class that knows how to write a stream format to an std::stream.
 * stream format is a format for stream recording - it holds the stream data samples
 * and the time they were taken. Those stream files can be played afterwards using
 * the StreamPlayer class.
 * In every time you will call operator () the functor will insert the sample and the
 * curr time to the std::stream.
 */
template <typename T>
class RecorderFunctor {
public:
	RecorderFunctor(boost::shared_ptr<std::ostream> out);

	T operator() (const T& data);

private:
	boost::shared_ptr<std::ostream> m_out;
	Timer m_timer;
};

/**
 * The Pop Stream Recorder class.
 * This is a stream filter that uses the RecorederFunctor (above) to record the data
 * passes in a pop stream.
 * It does not affect the data flow inside the stream, except for the time takes to
 * save the data.
 */
template <typename data_t>
class RecorderPopFilter : public FuncPopFilter<data_t> {
public:
	RecorderPopFilter(boost::shared_ptr<std::ostream> out, boost::shared_ptr<DataPopStream<data_t> > generator);
};

/**
 * The Push Stream Recorder class.
 * This is a stream filter that uses the RecorederFunctor (above) to record the data
 * passes in a push stream.
 * It does not affect the data flow inside the stream, except for the time takes to
 * save the data.
 */
template <typename data_t>
class RecorderPushFilter : public FuncPushFilter<data_t> {
public:
	RecorderPushFilter(boost::shared_ptr<std::ostream>, boost::shared_ptr<DataPushStream<data_t> > collector);
};

}  // namespace filter
}  // namespace stream

#include "stream_recorder.inl"

#endif /* STREAM_RECORDER_H_ */
