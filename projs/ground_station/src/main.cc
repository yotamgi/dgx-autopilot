#include "map_stream_view.h"
#include "3d_stream_view.h"
#include "size_stream_view.h"
#include "size_push_gen.h"
#include "orientation_stream_view.h"
#include "label_stream_view.h"
#include "stream/stream_utils.h"
#include <QtGui>

class SimplePosStream : public stream::DataPopStream<lin_algebra::vec2f> {
public:
	SimplePosStream(): m_num(0.) {}

	lin_algebra::vec2f get_data()  {

		lin_algebra::vec2f pos =
				lin_algebra::create_vec2f(3908560 + m_num*10., 3888820 + m_num*10.);
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

		lin_algebra::vec3f vec = lin_algebra::create_vec3f(m_num/5, m_num*2, m_num);
		m_num += 0.1;
		return vec;
	}

private:
	float m_num;
};


int main(int argc, char** argv) {
	boost::shared_ptr<stream::PushToPopConv<float> > push_size_stream(new stream::PushToPopConv<float>(0.));
	boost::shared_ptr<SimplePosStream> pos_stream(new SimplePosStream);
	boost::shared_ptr<SimpleVecStream> vec_stream(new SimpleVecStream);
	QApplication app(argc, argv);

	QWidget* window = new QWidget();

	QGridLayout* layout = new QGridLayout();

	gs::MapStreamView* map_view = new gs::MapStreamView(pos_stream, 0.2, QSize(400, 300), "./data/map");
	gs::StreamView3d* view3d = new gs::StreamView3d(0.1, QSize(400, 300));
	gs::SizeStreamView* size = new gs::SizeStreamView(push_size_stream, "stream", 0.1, -10., 10.);
	gs::LabelStreamView<lin_algebra::vec3f>* label = new gs::LabelStreamView<lin_algebra::vec3f>(vec_stream, 0.1, "pos");
	gs::SizePushGen* size_gen = new gs::SizePushGen(push_size_stream, "stream", -10., 10., 2.);

	gs::OrientationStreamView* orientation =
			new gs::OrientationStreamView(vec_stream, 0.1, 400);

	layout->addWidget(map_view, 0, 0);
	layout->addWidget(view3d, 0, 1);
	layout->addWidget(size, 0, 2);
	layout->addWidget(size_gen, 0, 3);
	layout->addWidget(orientation, 1, 0);
	layout->addWidget(label, 1, 1);

	window->setLayout(layout);
	window->show();
	view3d->addVecStream(vec_stream);
	view3d->addAngleStream(vec_stream);
	view3d->start();
	size->start();
	label->start();

	return app.exec();
}
