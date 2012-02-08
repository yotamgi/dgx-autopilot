#include "simulator_accelerometer.h"

namespace simulator {

typedef irr::core::vector3df irrvec3f;

SimulatorAccelerometerSensor::SimulatorAccelerometerSensor(irr::core::vector3df sensor_orientation):
		m_sensor_orientation(sensor_orientation),
		m_traced_object(NULL),
		m_prev_time_delta(0.)
{}

void SimulatorAccelerometerSensor::setSensedObject(irr::scene::ISceneNode *object) {
	m_traced_object = object;
}

void SimulatorAccelerometerSensor::update(float time_delta) {
	if (!m_traced_object) throw std::runtime_error("No sensed object in the sensor");

	irr::core::matrix4 sensor_rot;
	sensor_rot.setRotationDegrees(m_sensor_orientation);
	irr::core::matrix4 obj_trans = m_traced_object->getAbsoluteTransformation();
	irr::core::matrix4 trans  = obj_trans * sensor_rot;

	// calculate the acceleration
	irrvec3f curr_pos = m_traced_object->getPosition();
	irrvec3f curr_speed = (curr_pos - m_old_pos)/time_delta;
	irrvec3f acceleration = (curr_speed - m_old_speed) / (time_delta);


	m_old_pos = curr_pos;
	m_old_speed = curr_speed;
	m_prev_time_delta = time_delta;

	// add the g
	irrvec3f g(0, -1., 0);
	irrvec3f acc_g = g - 0.1*acceleration;

	// move it to the correct coordinate system
	float acc_g_len = acc_g.getLength();
	trans.getTransposed().rotateVect(acc_g);
	acc_g = acc_g.normalize() * acc_g_len; // rotateVect doesn't maintain size

	m_acc_data = lin_algebra::create_vec3f(acc_g.X + lin_algebra::frand()*0.05,
										   acc_g.Y + lin_algebra::frand()*0.05,
										   acc_g.Z + lin_algebra::frand()*0.05);
}

lin_algebra::vec3f SimulatorAccelerometerSensor::get_data() {
	return m_acc_data;
}


}  // namespace simulator
