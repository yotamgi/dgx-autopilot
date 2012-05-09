#include "size_push_gen.h"
#include <sstream>
#include <qt4/QtGui/qgroupbox.h>

namespace gs {

SizePushGen::SizePushGen(boost::shared_ptr<SizePushGen::size_stream> size_stream,
						 std::string stream_name,
		  	   	   	   	 float min_size,
		  	   	   	   	 float max_size,
		  	   	   	   	 float start_from,
		  	   	   	   	 SizePushGen::Style style):
		  m_size_stream(size_stream)
{
	// create the wanted bar
	switch(style) {
		case VERTICAL_DIAGRAM:
			m_slider = new QSlider();
			m_slider->setOrientation(Qt::Vertical);
			break;
		case HORIZONTAL_DIAGRAM:
			m_slider = new QSlider();
			m_slider->setOrientation(Qt::Horizontal);
			break;
	}

	// calculate the coefficient
	m_coeffecient = 100./(max_size - min_size);
	m_min = min_size;

	// configure the bar
	m_slider->setMinimum(0);
	m_slider->setMaximum(100);
	m_slider->setValue(m_min + start_from*m_coeffecient);
	m_slider->show();

	// place the progress bar in the widget
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_slider);
	QGroupBox* gb = new QGroupBox(tr(stream_name.c_str()));
	gb->setLayout(layout);
	QVBoxLayout* gb_layout = new QVBoxLayout();
	gb_layout->addWidget(gb);
	setLayout(gb_layout);

	m_size_stream->set_data(start_from);

	// the value_changede signal
	connect(m_slider, SIGNAL(sliderMoved(int)),
			this, SLOT(value_changed(int)));
}

void SizePushGen::value_changed(int value) {
	m_size_stream->set_data(m_min + value/m_coeffecient);
}

} // namespace gs
