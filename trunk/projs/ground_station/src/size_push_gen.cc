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
	// create the widgets
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
	m_input = new QDoubleSpinBox;

	// calculate the coefficient
	m_coeffecient = 100./(max_size - min_size);
	m_min = min_size;

	// configure the bar
	m_slider->setRange(0, 100);
	m_slider->setValue((start_from - m_min)*m_coeffecient);
	m_slider->setTracking(true);
	m_slider->setTickPosition(QSlider::TicksLeft);

	// configure the input
	m_input->setRange(min_size, max_size);
	m_input->setValue(start_from);

	// place the progress bar in the widget
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(new QLabel(tr(stream_name.c_str())));
	layout->addWidget(m_slider);
	layout->addWidget(m_input);
	setLayout(layout);

	m_size_stream->set_data(start_from);

	// the value_changed signals
	connect(m_slider, SIGNAL(sliderMoved(int)),
			this, SLOT(value_changed(int)));

	connect(m_slider, SIGNAL(valueChanged(int)),
			this, SLOT(value_changed(int)));

	connect(m_input, SIGNAL(editingFinished()),
			this, SLOT(input_value_changed()));
}

void SizePushGen::value_changed(int value) {
	m_size_stream->set_data(m_min + value/m_coeffecient);
	m_input->setValue(m_min + value/m_coeffecient);
}

void SizePushGen::input_value_changed() {
	float value = m_input->value();
	m_slider->setValue((value - m_min)*m_coeffecient); // which emits the value changed signal
}

float SizePushGen::get_value() {
	return m_min + m_slider->value()/m_coeffecient;
}

void SizePushGen::set_value(float value){
	m_slider->setValue((value - m_min)*m_coeffecient); // which emits the value changed signal
}

} // namespace gs
