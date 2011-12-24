
#ifndef DIRECTIONPRESENTER_H_
#define DIRECTIONPRESENTER_H_

#include <stream/data_pop_stream.h>
#include <stream/util/lin_algebra.h>
#include <irrlicht/irrlicht.h>
#include <boost/thread.hpp>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <QtGui>
#include <QtOpenGL/qgl.h>

namespace gs {

class StreamView3d : public QGLWidget {
	Q_OBJECT
public:
	typedef stream::DataPopStream<float> streamf;
	typedef boost::shared_ptr<stream::DataPopStream<float> > streamfptr;
	typedef stream::DataPopStream<lin_algebra::vec3f> stream3;
	typedef boost::shared_ptr<stream3> stream3ptr;

	StreamView3d(float update_time, QSize size, QWidget* parent = 0);
	~StreamView3d();

	void addAngleStream(stream3ptr angle_stream, irr::core::vector3df pos = irr::core::vector3df(0., 0., 0.));
	void addVecStream(stream3ptr vec_stream, irr::core::vector3df pos = irr::core::vector3df(0., 0., 0.));
	void addSizeStream(streamfptr size_stream,
			irr::core::vector2df scale = irr::core::vector2df(1., 1.),
			irr::core::vector2df pos = irr::core::vector2df(0., 0.));

	void start();


public slots:

	void update();

protected:

	void paintEvent(QPaintEvent* event);

private:

	class AnglePresenter {
	public:
		AnglePresenter(stream3ptr angle_stream, irr::core::vector3df pos);

		void initalize(irr::IrrlichtDevice* m_device);
		void draw(irr::IrrlichtDevice* m_device);

	private:
		stream3ptr m_angle_stream;
		irr::core::vector3df m_pos;
		irr::scene::ISceneNode * m_object;
	};

	class VecPresenter {
	public:
		VecPresenter(stream3ptr vec_stream, irr::core::vector3df pos);

		void initalize(irr::IrrlichtDevice* m_device);
		void draw(irr::IrrlichtDevice* m_device);

	private:
		stream3ptr m_vec_stream;
		irr::core::vector3df m_pos;
		irr::scene::ISceneNode * m_object;
		irr::core::vector3df m_scale;
	};

	class SizePresenter {
	public:
		SizePresenter(streamfptr size_stream, irr::core::vector2df pos, irr::core::vector2df scale);

		void initalize(irr::IrrlichtDevice* m_device);
		void draw(irr::IrrlichtDevice* m_device);

	private:
		streamfptr m_size_stream;
		irr::core::vector2df m_pos;
		irr::core::vector2df m_scale;
	};

	irr::IrrlichtDevice* m_device;

	std::vector<boost::shared_ptr<AnglePresenter> > m_angles;
	std::vector<boost::shared_ptr<VecPresenter> > m_vecs;
	std::vector<boost::shared_ptr<SizePresenter> > m_sizes;

	QTimer* m_timer;
	float m_update_time;
};

} // namespace gs

#endif /* DIRECTIONPRESENTER_H_ */
