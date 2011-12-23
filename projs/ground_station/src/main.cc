#include "map_stream_view.h"
#include "3d_stream_view.h"
#include <QtGui>

class SimplePosStream : public stream::DataPopStream<lin_algebra::vec2f> {
public:
	SimplePosStream(): m_num(0.) {}

	lin_algebra::vec2f get_data()  {

		lin_algebra::vec2f pos;
		pos[0] = m_num;
		pos[1] = m_num/5.;
		m_num -= 1.0;
		return pos;
	}

private:
	float m_num;
};


int main(int argc, char** argv) {

	boost::shared_ptr<SimplePosStream> pos_stream(new SimplePosStream);
	QApplication app(argc, argv);

	QWidget* window = new QWidget();

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(new QPushButton("&A"));

	gs::MapStreamView* map_view = new gs::MapStreamView(pos_stream, 0.2, QSize(400, 300), "./data/map", "ogr");
	gs::StreamView3d* view3d = new gs::StreamView3d(0.1, QSize(400, 300));

	layout->addWidget(map_view);
	layout->addWidget(view3d);

	window->setLayout(layout);
	window->show();
	view3d->start();

	return app.exec();
}
