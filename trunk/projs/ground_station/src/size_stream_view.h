
#ifndef SIZE_STREAM_VIEW_H_
#define SIZE_STREAM_VIEW_H_

#include <stream/data_pop_stream.h>
#include <boost/shared_ptr.hpp>
#include <QtGui>

namespace gs {

typedef stream::DataPopStream<float> size_stream;

class SizeStreamView : public QWidget {
public:
	SizeStreamView(boost::shared_ptr<size_stream> size_stream,
			  	   float update_time,
	   	   	   	   float max_size,
	   	   	   	   float min_size);

	void start();
protected:

	void timerEvent(QTimerEvent *);

private:

	float m_coeffecient;

	float m_upadte_time;
	boost::shared_ptr<size_stream> m_size_stream;

	QProgressBar* m_progress_bar;
};

} // namespace gs


#endif /* SIZE_STREAM_VIEW_H_ */
