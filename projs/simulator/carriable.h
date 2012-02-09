/*
 * sensor.h
 *
 *  Created on: Feb 4, 2012
 *      Author: yotam
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <irrlicht/irrlicht.h>

namespace simulator {

class Carriable {
public:
	virtual void setSensedObject(irr::scene::ISceneNode *object) = 0;

	virtual void update(float time_delta) = 0;
};

}  // namespace simulator

#endif /* SENSOR_H_ */
