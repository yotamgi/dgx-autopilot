#ifndef SIMULATOR_ACCELEROMETER_H_
#define SIMULATOR_ACCELEROMETER_H_

#include "carriable.h"
#include <stream/util/lin_algebra.h>

namespace simulator {


class SimulatorAccelerometerSensor : public Carriable, public stream::DataPopStream<lin_algebra::vec3f> {
public:

	SimulatorAccelerometerSensor(irr::core::vector3df sensor_orientation = irr::core::vector3df());

	void setSensedObject(irr::scene::ISceneNode *object);

	void update(float time_delta);

	lin_algebra::vec3f get_data();

private:

	irr::core::vector3df m_sensor_orientation;
	irr::scene::ISceneNode *m_traced_object;

	irr::core::vector3df m_old_pos;
	irr::core::vector3df m_old_speed;

	lin_algebra::vec3f m_acc_data;
};


}  // namespace simulator


#endif /* SIMULATOR_ACCELEROMETER_H_ */
