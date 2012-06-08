#ifndef LABELVIEW_H_
#define LABELVIEW_H_

#include <stream/data_pop_stream.h>
#include <boost/shared_ptr.hpp>
#include <QtGui/QtGui>

namespace gs {


template <typename T>
class LabelStreamView : public QLabel {
public:
	typedef stream::DataPopStream<T> data_stream;

	LabelStreamView(boost::shared_ptr<data_stream> stream,
			  	    float update_time,
			  	    std::string stream_name):
			 m_update_time(update_time),
			 m_data_stream(stream),
			 m_stream_name(stream_name)
	{}

	void start() {
		startTimer(1000*m_update_time);
	}

protected:

	void timerEvent(QTimerEvent *) {
		std::stringstream ss;
		ss << m_stream_name << ": " << m_data_stream->get_data();
		setText(tr(ss.str().c_str()));
	}

private:

	float m_update_time;
	boost::shared_ptr<data_stream> m_data_stream;
	std::string m_stream_name;
};

} // namespace gs

#endif /* LABELVIEW_H_ */
