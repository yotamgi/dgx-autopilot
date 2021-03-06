#ifndef ORIENTATION_WIDGET_H_
#define ORIENTATION_WIDGET_H_

#include "external_widgets/orientation_widget.h"
#include "external_widgets/heading_widget.h"
#include <stream/data_pop_stream.h>
#include <stream/util/lin_algebra.h>
#include <qt4/QtCore/qtimer.h>
#include <boost/shared_ptr.hpp>
#include <iostream>

namespace gs {

typedef stream::DataPopStream<lin_algebra::vec3f> orientation_stream;

class OrientationStreamView : public QWidget {
	Q_OBJECT
public:
	OrientationStreamView(boost::shared_ptr<orientation_stream> stream,
			float update_time,
			float width,
			QWidget *parent = NULL);

public slots:

	void update();

private:
	OrientationWidget* m_orientation_widget;
	HeadingWidget* m_heading_widget;
	QTimer m_timer;
	boost::shared_ptr<orientation_stream> m_stream;
};

} // namespace gs

#endif /* ORIENTATION_WIDGET_H_ */
