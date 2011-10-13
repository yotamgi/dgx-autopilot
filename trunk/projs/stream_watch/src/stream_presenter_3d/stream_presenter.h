
#ifndef DIRECTIONPRESENTER_H_
#define DIRECTIONPRESENTER_H_

#include <stream/generators.h>
#include <irrlicht/irrlicht.h>
#include <boost/thread.hpp>
#include <stdexcept>
#include <iostream>
#include <vector>

class StreamPresenter {
public:
	typedef stream::DataGenerator<float> streamf;
	typedef boost::shared_ptr<stream::DataGenerator<float> > streamfptr;
	typedef stream::VecGenerator<float,3> stream3;
	typedef boost::shared_ptr<stream::VecGenerator<float,3> > stream3ptr;

	StreamPresenter();
	~StreamPresenter();

	void addAngleStream(stream3ptr angle_stream, irr::core::vector3df pos = irr::core::vector3df(0., 0., 0.));
	void addVecStream(stream3ptr vec_stream, irr::core::vector3df pos = irr::core::vector3df(0., 0., 0.));
	void addSizeStream(streamfptr size_stream,
			irr::core::vector2df scale = irr::core::vector2df(1., 1.),
			irr::core::vector2df pos = irr::core::vector2df(0., 0.));

	void run(bool open_thread=true);

	void stop() {m_running = false; }

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

	boost::shared_ptr<boost::thread> m_thread;

	std::vector<boost::shared_ptr<AnglePresenter> > m_angles;
	std::vector<boost::shared_ptr<VecPresenter> > m_vecs;
	std::vector<boost::shared_ptr<SizePresenter> > m_sizes;

	volatile bool m_running;
};

#endif /* DIRECTIONPRESENTER_H_ */
