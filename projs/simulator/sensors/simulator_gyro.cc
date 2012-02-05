#include "simulator_gyro.h"

namespace simulator {

typedef irr::core::vector3df irrvec3f;

SimulatorGyroSensor::SimulatorGyroSensor(irr::core::vector3df sensor_orientation):
		m_sensor_orientation(sensor_orientation),
		m_traced_object(NULL),
		m_gyro_drift(lin_algebra::frand()*1., lin_algebra::frand()*1., lin_algebra::frand()*1.)
{}

void SimulatorGyroSensor::setSensedObject(irr::scene::ISceneNode *object) {
	m_traced_object = object;
}

void SimulatorGyroSensor::update(float time_delta) {
	if (!m_traced_object) throw std::runtime_error("No sensed object in the sensor");

	irr::core::matrix4 sensor_rot;
	sensor_rot.setRotationDegrees(m_sensor_orientation);

	irrvec3f obj_rot = m_traced_object->getRotation();
	irr::core::matrix4 transformation;
	transformation.setRotationDegrees(obj_rot);
	transformation *= sensor_rot;
	irr::core::matrix4 trans_inverse;
	m_prev_trans.getInverse(trans_inverse);
	irrvec3f angle_vel = (trans_inverse * transformation).getRotationDegrees();
	m_prev_trans = transformation;

	if (angle_vel.X < -300) angle_vel.X += 360.;
	if (angle_vel.X > 300)  angle_vel.X -= 360.;
	if (angle_vel.Y < -300) angle_vel.Y += 360.;
	if (angle_vel.Y > 300)  angle_vel.Y -= 360.;
	if (angle_vel.Z < -300) angle_vel.Z += 360.;
	if (angle_vel.Z > 300)  angle_vel.Z -= 360.;

	angle_vel /= time_delta;

	// update the gyro
	m_gyro_data[0] = angle_vel.X + lin_algebra::frand()*2.0 + 2*m_gyro_drift.X;
	m_gyro_data[1] = angle_vel.Y + lin_algebra::frand()*2.0 + 2*m_gyro_drift.Y;
	m_gyro_data[2] = angle_vel.Z + lin_algebra::frand()*2.0 + 2*m_gyro_drift.Z;
}

lin_algebra::vec3f SimulatorGyroSensor::get_data() {
	return m_gyro_data;
}


}  // namespace simulator
