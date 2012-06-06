#include "orientation_stream_view.h"
#include <qt4/QtGui/qboxlayout.h>

namespace gs {


OrientationStreamView::OrientationStreamView(boost::shared_ptr<orientation_stream> stream,
											float update_time,
											float width,
											QWidget *parent):
		QWidget(parent),
		m_timer(this),
		m_stream(stream)
{
	setFixedWidth(width);
	setFixedHeight(width/2.);

	m_orientation_widget = new OrientationWidget(this);
	m_orientation_widget->setFixedWidth(width/2.);
	m_orientation_widget->setFixedHeight(height());
	m_orientation_widget->init();

	m_heading_widget = new HeadingWidget(this);
	m_heading_widget->setFixedWidth(width/2.);
	m_heading_widget->setFixedHeight(height());
	m_heading_widget->init();

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(m_orientation_widget);
	layout->addWidget(m_heading_widget);

	setLayout(layout);

 	connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer.start(1000*update_time);
}

void OrientationStreamView::update() {
	lin_algebra::vec3f orientation = m_stream->get_data();
	m_orientation_widget->setPitch(orientation[0]);
	m_orientation_widget->setRoll(orientation[2]);
	m_heading_widget->set(orientation[1]);
}

}  // namespace gs
