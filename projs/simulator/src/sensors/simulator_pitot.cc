#include "simulator_pitot.h"

namespace simulator {

typedef irr::core::vector3df irrvec3f;

lin_algebra::vec3f to_lin(irrvec3f vec) {
	return lin_algebra::create_vec3f(vec.X, vec.Y, vec.Z);
}

SimulatorPitotSensor::SimulatorPitotSensor(lin_algebra::vec3f sensor_heading, boost::shared_ptr<WindGen> simulator_wind):
		m_simulator_wind(simulator_wind),
		m_traced_object(NULL),
		m_sensor_heading(sensor_heading[0], sensor_heading[1], sensor_heading[2])
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

	irrvec3f curr_heading;
	m_traced_object->getAbsoluteTransformation().rotateVect(curr_heading, m_sensor_heading);
	curr_heading = curr_heading.normalize();

	m_pitot_data = lin_algebra::dot(airspeed, to_lin(curr_heading)) * curr_heading.getLength();

	m_old_pos = curr_pos;
}

float SimulatorPitotSensor::get_data() {
	return m_pitot_data;
}


}  // namespace simulator
