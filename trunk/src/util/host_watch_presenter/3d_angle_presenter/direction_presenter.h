
#ifndef DIRECTIONPRESENTER_H_
#define DIRECTIONPRESENTER_H_

#include <irrlicht/irrlicht.h>
#include <boost/thread.hpp>
#include <stdexcept>
#include <iostream>

struct angle3d_t {
	float ax;
	float ay;
	float az;
};

class DirectionPresenter {
public:
	DirectionPresenter();
	~DirectionPresenter();

	void set_angle(angle3d_t a) {	m_curr_angle = a; }

	void run(bool open_thread=true);

	void stop() {m_running = false; }

private:

	irr::IrrlichtDevice* m_device;

	irr::scene::ISceneNode * m_object;

	boost::shared_ptr<boost::thread> m_thread;

	angle3d_t m_curr_angle;

	bool m_running;

};

#endif /* DIRECTIONPRESENTER_H_ */
