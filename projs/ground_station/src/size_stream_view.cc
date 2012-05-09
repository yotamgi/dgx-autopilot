#include "size_stream_view.h"
#include <sstream>
#include <qt4/QtGui/qgroupbox.h>

namespace gs {

SizeStreamView::SizeStreamView(boost::shared_ptr<size_stream> size_stream,
							   std::string stream_name,
		  	   	   	   	   	   float update_time,
		  	   	   	   	   	   float min_size,
		  	   	   	   	   	   float max_size,
		  	   	   	   	   	   SizeStreamView::Style style):
		  m_upadte_time(update_time),
		  m_size_stream(size_stream)
{
	// create the wanted bar
	switch(style) {
		case VERTICAL_DIAGRAM:
			m_bar = new QProgressBar();
			m_bar->setOrientation(Qt::Vertical);
			break;
		case HORIZONTAL_DIAGRAM:
			m_bar = new QProgressBar();
			m_bar->setOrientation(Qt::Horizontal);
			break;
	}

	// calculate the coefficient
	float range = max_size - min_size;
	m_coeffecient = 100./range;

	// configure the bar
	m_bar->setMinimum(min_size*m_coeffecient);
	m_bar->setMaximum(max_size*m_coeffecient);
	m_bar->setValue(min_size*m_coeffecient);
	m_bar->show();

	// place the progress bar in the widget
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_bar);
	QGroupBox* gb = new QGroupBox(tr(stream_name.c_str()));
	gb->setLayout(layout);
	QVBoxLayout* gb_layout = new QVBoxLayout();
	gb_layout->addWidget(gb);
	setLayout(gb_layout);
}

void SizeStreamView::start() {
	startTimer(1000*m_upadte_time);
}

void SizeStreamView::timerEvent(QTimerEvent *) {
	float data = m_size_stream->get_data();
	m_bar->setValue(data * m_coeffecient);
	std::stringstream ss;
	ss << data;
	m_bar->setFormat(ss.str().c_str());
}


} // namespace gs
