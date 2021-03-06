#include "stream_recorder.h"
#include <boost/shared_ptr.hpp>
#include <typeinfo>

namespace stream {
namespace filters {

template <typename T>
inline RecorderFunctor<T>::RecorderFunctor(boost::shared_ptr<std::ostream> out):m_out(out)
{
	if (!m_out->good()) {
		throw std::runtime_error("Recording std::stream is not valid (Recorder Constructor)");
	}

	// create a little header
	(*m_out) << "STREAM" << std::endl;
	(*m_out) << typeid(T).name() << std::endl;

}

template <typename T>
inline T RecorderFunctor<T>::operator() (const T& data) {

	if (!m_out->good()) {
		throw std::runtime_error("Recording std::stream is not valid");
	}

	(*m_out) << m_timer.passed() << "," << data << std::endl;
	return data;
}

template <typename data_t>
RecorderPopFilter<data_t>::RecorderPopFilter(boost::shared_ptr<std::ostream> out,
									 boost::shared_ptr<DataPopStream<data_t> > generator):
				FuncPopFilter<data_t>(generator, RecorderFunctor<data_t>(out))
{}

template <typename data_t>
RecorderPushFilter<data_t>::RecorderPushFilter(boost::shared_ptr<std::ostream> out,
									 boost::shared_ptr<DataPushStream<data_t> > collector):
				FuncPushFilter<data_t>(collector, RecorderFunctor<data_t>(out))
{}

}  // namespace filter
}  // namespace stream

