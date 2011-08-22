
#ifndef DIRECTIONPRESENTER_H_
#define DIRECTIONPRESENTER_H_

#include <irrlicht/irrlicht.h>
#include <stdexcept>

struct angle3d_t {
	float ax;
	float ay;
	float az;
};

class DirectionPresenter {
public:
	DirectionPresenter();
	~DirectionPresenter();

	void set_angle(angle3d_t a) { m_curr_angle = a; }

	void run();

private:

	irr::IrrlichtDevice* m_device;

	irr::scene::ISceneNode * m_object;

	angle3d_t m_curr_angle;

};

#endif /* DIRECTIONPRESENTER_H_ */
