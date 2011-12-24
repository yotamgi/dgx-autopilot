#include "size_stream_view.h"

namespace gs {

SizeStreamView::SizeStreamView(boost::shared_ptr<size_stream> size_stream,
		  	   	   	   	   	   float update_time,
		  	   	   	   	   	   float min_size,
		  	   	   	   	   	   float max_size):
		  m_upadte_time(update_time),
		  m_size_stream(size_stream),
		  m_progress_bar(new QProgressBar(this))
{
	// calculate the coefficient
	float range = max_size - min_size;
	m_coeffecient = 100./range;

	m_progress_bar->setMinimum(min_size*m_coeffecient);
	m_progress_bar->setMaximum(max_size*m_coeffecient);
	m_progress_bar->setValue(min_size*m_coeffecient);
	m_progress_bar->setOrientation(Qt::Vertical);
	m_progress_bar->show();

	// place the progress bar in the widget
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_progress_bar);
	setLayout(layout);
}

void SizeStreamView::start() {
	startTimer(1000*m_upadte_time);
}

void SizeStreamView::timerEvent(QTimerEvent *) {
	float data = m_size_stream->get_data();
	m_progress_bar->setValue(data * m_coeffecient);
}


} // namespace gs
