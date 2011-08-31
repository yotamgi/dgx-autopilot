/*
 * flying_object.h
 *
 *  Created on: Aug 20, 2011
 *      Author: yotam
 */

#ifndef FLYING_OBJECT_H_
#define FLYING_OBJECT_H_

#include <irrlicht/irrlicht.h>
#include <boost/shared_ptr.hpp>

namespace simulator {

/**
 * Retpresents a flying object in space.
 */
class FlyingObject {
public:
	FlyingObject(irr::IrrlichtDevice* device):m_device(device) {}
	virtual ~FlyingObject() {}

	virtual void update(float timeDelta) =0;

	virtual irr::core::vector3df get_pos() const  =0;

protected:

	irr::IrrlichtDevice* m_device;
};

} // namespace simulator


#endif /* FLYING_OBJECT_H_ */
