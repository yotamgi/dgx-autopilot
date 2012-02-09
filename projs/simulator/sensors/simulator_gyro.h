#ifndef SIMULATOR_GYRO_H_
#define SIMULATOR_GYRO_H_

#include <stream/util/lin_algebra.h>
#include <stdexcept>
#include "carriable.h"

namespace simulator {


class SimulatorGyroSensor : public Carriable, public stream::DataPopStream<lin_algebra::vec3f> {
public:

	SimulatorGyroSensor(irr::core::vector3df sensor_orientation = irr::core::vector3df());

	void setSensedObject(irr::scene::ISceneNode *object);

	void update(float time_delta);

	lin_algebra::vec3f get_data();

private:

	irr::core::vector3df m_sensor_orientation;
	irr::scene::ISceneNode *m_traced_object;

	irr::core::matrix4 m_prev_trans;

	irr::core::vector3df m_gyro_drift;

	lin_algebra::vec3f m_gyro_data;
};


}  // namespace simulator

#endif /* SIMULATOR_GYRO_H_ */
