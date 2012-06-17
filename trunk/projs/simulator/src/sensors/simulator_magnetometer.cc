#include "simulator_magnetometer.h"

namespace simulator {

typedef irr::core::vector3df irrvec3f;

SimulatorMagnetometerSensor::SimulatorMagnetometerSensor(irr::core::vector3df sensor_orientation):
		m_sensor_orientation(sensor_orientation),
		m_traced_object(NULL)
{}

void SimulatorMagnetometerSensor::setSensedObject(irr::scene::ISceneNode *object) {
	m_traced_object = object;
}

void SimulatorMagnetometerSensor::update(float time_delta) {
	if (!m_traced_object) throw std::runtime_error("No sensed object in the sensor");

	irr::core::matrix4 sensor_rot;
	sensor_rot.setRotationDegrees(m_sensor_orientation);
	irr::core::matrix4 obj_trans = m_traced_object->getAbsoluteTransformation();
	irr::core::matrix4 trans  = obj_trans * sensor_rot;

	// create the vector
	irrvec3f north(-1., -1., 0);
	north *= 10.;

	// move it to the correct coordinate system
	float north_len = north.getLength();
	trans.getTransposed().rotateVect(north);
	north = north.normalize() * north_len; // rotateVect doesn't maintain size

	m_mag_data = lin_algebra::create_vec3f(north.X + lin_algebra::frand()*0.05,
										   north.Y + lin_algebra::frand()*0.05,
										   north.Z + lin_algebra::frand()*0.05);
}

lin_algebra::vec3f SimulatorMagnetometerSensor::get_data() {
	return m_mag_data;
}


}  // namespace simulator
