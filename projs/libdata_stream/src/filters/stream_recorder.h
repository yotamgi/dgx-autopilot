#ifndef STREAM_RECORDER_H_
#define STREAM_RECORDER_H_

#include <stream/func_filter.h>
#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <util/time.h>
#include <ostream>
#include <string>

namespace stream {
namespace filters {

template <typename T>
class RecorderFunctor {
public:
	RecorderFunctor(std::ostream& out);

	T operator() (const T& data);

private:
	std::ostream& m_out;
	Timer m_timer;
};

template <typename data_t>
class RecorderPopFilter : public FuncPopFilter<data_t> {
public:
	RecorderPopFilter(std::ostream& out, boost::shared_ptr<DataPopStream<data_t> > generator);
};


}  // namespace filter
}  // namespace stream

#include "stream_recorder.inl"

#endif /* STREAM_RECORDER_H_ */
