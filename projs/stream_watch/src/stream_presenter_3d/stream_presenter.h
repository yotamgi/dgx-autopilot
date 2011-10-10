
#ifndef DIRECTIONPRESENTER_H_
#define DIRECTIONPRESENTER_H_

#include <stream/generators.h>
#include <irrlicht/irrlicht.h>
#include <boost/thread.hpp>
#include <stdexcept>
#include <iostream>

class StreamPresenter {
public:
	StreamPresenter();
	~StreamPresenter();

	void setAngleStream(boost::shared_ptr<stream::VecGenerator<float,3> > angle_stream);
	void setVecStream(boost::shared_ptr<stream::VecGenerator<float,3> > vec_stream);

	void run(bool open_thread=true);

	void stop() {m_running = false; }

private:

	irr::IrrlichtDevice* m_device;

	irr::scene::ISceneNode * m_object;

	boost::shared_ptr<boost::thread> m_thread;

	boost::shared_ptr<stream::VecGenerator<float,3> > m_angle;
	boost::shared_ptr<stream::VecGenerator<float,3> > m_vec;

	volatile bool m_running;
};

#endif /* DIRECTIONPRESENTER_H_ */
