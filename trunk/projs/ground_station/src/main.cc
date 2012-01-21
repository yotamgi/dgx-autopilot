#include "map_stream_view.h"
#include "3d_stream_view.h"
#include "size_stream_view.h"
#include <QtGui>

class SimpleSizeStream : public stream::DataPopStream<float> {
public:
	SimpleSizeStream(): m_num(0.) {}

	float get_data()  {
		m_num += 0.1;
		return m_num;
	}

private:
	float m_num;
};

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

class SimpleVecStream : public stream::DataPopStream<lin_algebra::vec3f> {
public:
	SimpleVecStream(): m_num(0.) {}

	lin_algebra::vec3f get_data()  {

		lin_algebra::vec3f vec;
		vec[0] = m_num;
		vec[1] = m_num/5.;
		vec[2] = m_num/50.;
		m_num += 0.1;
		return vec;
	}

private:
	float m_num;
};


int main(int argc, char** argv) {

	boost::shared_ptr<SimpleSizeStream> size_stream(new SimpleSizeStream);
	boost::shared_ptr<SimplePosStream> pos_stream(new SimplePosStream);
	boost::shared_ptr<SimpleVecStream> vec_stream(new SimpleVecStream);
	QApplication app(argc, argv);

	QWidget* window = new QWidget();

	QHBoxLayout* layout = new QHBoxLayout();

	gs::MapStreamView* map_view = new gs::MapStreamView(pos_stream, 0.2, QSize(400, 300), "./data/map");
	gs::StreamView3d* view3d = new gs::StreamView3d(0.1, QSize(400, 300));
	gs::SizeStreamView* size = new gs::SizeStreamView(size_stream, 0.1, 0., 10.);

	layout->addWidget(map_view);
	layout->addWidget(view3d);
	layout->addWidget(size);

	window->setLayout(layout);
	window->show();
	view3d->addVecStream(vec_stream);
	view3d->addAngleStream(vec_stream);
	view3d->start();
	size->start();

	return app.exec();
}
