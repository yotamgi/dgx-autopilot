#include "simulator_pitot.h"

namespace simulator {

typedef irr::core::vector3df irrvec3f;

SimulatorPitotSensor::SimulatorPitotSensor(boost::shared_ptr<WindGen> simulator_wind):
		m_simulator_wind(simulator_wind),
		m_traced_object(NULL)
{}

void SimulatorPitotSensor::setSensedObject(irr::scene::ISceneNode *object) {
	m_traced_object = object;
}

void SimulatorPitotSensor::update(float time_delta) {
	if (!m_traced_object) throw std::runtime_error("No sensed object in the sensor");

	// calculate the plain speed
	irrvec3f curr_pos = m_traced_object->getPosition();
	irrvec3f ground_speed = (curr_pos - m_old_pos)/time_delta;

	lin_algebra::vec3f airspeed =
			lin_algebra::create_vec3f(ground_speed.X, ground_speed.Y, ground_speed.Z) -
			m_simulator_wind->get_wind();

	m_pitot_data = lin_algebra::vec_len(airspeed);

	m_old_pos = curr_pos;
}

float SimulatorPitotSensor::get_data() {
	return m_pitot_data;
}


}  // namespace simulator
