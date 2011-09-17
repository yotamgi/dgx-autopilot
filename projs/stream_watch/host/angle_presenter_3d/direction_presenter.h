
#ifndef DIRECTIONPRESENTER_H_
#define DIRECTIONPRESENTER_H_

#include <irrlicht/irrlicht.h>
#include <boost/thread.hpp>
#include <stdexcept>
#include <iostream>

struct vector3_t {
	float ax;
	float ay;
	float az;
};

struct angle3d_t : vector3_t {};

class DirectionPresenter {
public:
	DirectionPresenter();
	~DirectionPresenter();

	void show_angle(bool b) { m_show_object = b; }
	void set_angle(angle3d_t a) {	m_curr_angle = a; }

	void show_vec(bool b) { m_show_arrow = b; }
	void set_vec(vector3_t vec) { m_curr_vec = vec; }

	void run(bool open_thread=true);

	void stop() {m_running = false; }

private:

	irr::IrrlichtDevice* m_device;

	irr::scene::ISceneNode * m_object;
	irr::scene::ISceneNode * m_arrow;

	boost::shared_ptr<boost::thread> m_thread;

	angle3d_t m_curr_angle;
	vector3_t m_curr_vec;

	volatile bool m_running;

	bool m_show_arrow;
	bool m_show_object;
};

#endif /* DIRECTIONPRESENTER_H_ */
