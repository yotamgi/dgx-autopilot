#include "orientation_stream_view.h"

namespace gs {


OrientationStreamView::OrientationStreamView(boost::shared_ptr<orientation_stream> stream,
											float update_time,
											QSize size,
											QWidget *parent):
		QWidget(parent),
		m_timer(this),
		m_stream(stream)
{
	if (size.width() != 0) setFixedWidth(size.width());
	if (size.height() != 0) setFixedHeight(size.height());

	m_orientation_widget = new OrientationWidget(this);
	m_orientation_widget->setFixedWidth(width());
	m_orientation_widget->setFixedHeight(height());
	m_orientation_widget->init();

 	connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer.start(1000*update_time);
}

void OrientationStreamView::update() {
	lin_algebra::vec3f orientation = m_stream->get_data();
	m_orientation_widget->setPitch(orientation[0]);
	m_orientation_widget->setRoll(orientation[2]);
}

}  // namespace gs
