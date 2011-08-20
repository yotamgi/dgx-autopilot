/*
 * plane.h
 *
 *  Created on: Aug 20, 2011
 *      Author: yotam
 */

#ifndef PLANE_H_
#define PLANE_H_

#include "flying_object.h"
#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace simulator {

struct PlanesParams {

};

class Plane : public FlyingObject {
public:
	Plane(irr::IrrlichtDevice* device,
			irr::core::vector3df start_pos);

	virtual void update(float time_delta);

	virtual irr::core::vector3df get_pos() { return m_object->getPosition(); }

private:

	irr::core::vector3df m_direction;

	irr::scene::ISceneNode * m_object;

};

}  // namespace simulator


#endif /* PLANE_H_ */

