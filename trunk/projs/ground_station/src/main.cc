#include "map_stream_view.h"
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

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(new QPushButton("&A"));
	layout->addWidget(new QPushButton("&B"));

	gs::MapStreamView* map_view = new gs::MapStreamView("./data/map", "ogr", pos_stream, 0.2);
	layout->addWidget(map_view);

	QWidget* window = new QWidget();
	window->setLayout(layout);
	window->show();

	return app.exec();
}
