#include "simulator_altmeter.h"

namespace simulator {

typedef irr::core::vector3df irrvec3f;

SimulatorAltmeterSensor::SimulatorAltmeterSensor():
		m_traced_object(NULL)
{}

void SimulatorAltmeterSensor::setSensedObject(irr::scene::ISceneNode *object) {
	m_traced_object = object;
}

void SimulatorAltmeterSensor::update(float time_delta) {
	if (!m_traced_object) throw std::runtime_error("No sensed object in the sensor");

	m_alt_data = m_traced_object->getPosition().Y;
}

float SimulatorAltmeterSensor::get_data() {
	return m_alt_data;
}


}  // namespace simulator
