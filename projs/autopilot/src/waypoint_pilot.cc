#include "waypoint_pilot.h"
#include <cmath>

namespace autopilot {

WaypointPilot::WaypointPilot(const WaypointPilotParams& params, boost::shared_ptr<NormalPlainCockpit> cockpit):
		m_cockpit(cockpit),
		m_running(false),
		m_params(params)
{}

void WaypointPilot::start(bool open_thread) {
	m_running = true;
	if (open_thread) {
		m_running_thread = boost::thread(&WaypointPilot::start, this, false);
	} else {
		fly();
	}
}
void WaypointPilot::stop() {
	m_running = false;
	m_running_thread.join();
}

void WaypointPilot::fly() {
	vec_stream_ptr oreintation = m_cockpit->orientation();
	vec_stream_ptr position = m_cockpit->position();
	vec_stream_ptr speed = m_cockpit->speed();
	servo_stream_ptr tilt = m_cockpit->tilt_servo();
	servo_stream_ptr pitch = m_cockpit->pitch_servo();

	while (m_running) {

		// calculate the planes position, altitude and orientation
		lin_algebra::vec3f plain_orientation = oreintation->get_data();
		std::cout << "Orientation " << plain_orientation << std::endl;
		lin_algebra::vec3f plain_pos_and_alt = position->get_data();
		lin_algebra::vec2f plain_pos;
		plain_pos[0] = plain_pos_and_alt[0]; plain_pos[1] = plain_pos_and_alt[2];
		float plain_alt = plain_pos_and_alt[1];

		// understand the wanted direction and altitude
		lin_algebra::vec2f wanted_diretion = m_target - lin_algebra::normalize(plain_pos);
		float delta_alt = m_altitude - plain_alt;

		float expected_pitch;
		if (delta_alt >  20.) 		expected_pitch = std::sin(m_params.avg_climbing_angle);
		else if (delta_alt < -20.) 	expected_pitch = std::sin(m_params.avg_climbing_angle);
		else expected_pitch = delta_alt/20. / 4.;
		expected_pitch = std::max(expected_pitch,  std::sin(m_params.max_climbing_angle));
		expected_pitch = std::min(expected_pitch, -std::sin(m_params.max_decending_angle));

		float expected_tilt = std::tan(wanted_diretion[1]/wanted_diretion[0])/lin_algebra::PI;
		expected_tilt = std::max(expected_tilt,  std::sin(m_params.max_tilt_angle));
		expected_tilt = std::min(expected_tilt, -std::sin(m_params.max_tilt_angle));

		std::cout << "The pitch angle is " << plain_orientation[0] << "And the expected one is " << expected_pitch << std::endl;
		std::cout << "The tilt angle is  " << plain_orientation[2] << "And the expected one is " << expected_tilt << std::endl;

		// control the stick to achieve the speed vector we calculated
		float pitch_data = plain_orientation[0]/180. - expected_pitch;
		float tilt_data  = plain_orientation[2]/180. - expected_tilt;
		m_cockpit->pitch_servo()->set_data(50. + 50. * pitch_data);
		m_cockpit->tilt_servo()->set_data(50. + 50. * tilt_data);
	}
}

void WaypointPilot::to_waypoint(lin_algebra::vec2f target, float altitude) {
	m_target = target;
	if (altitude > 0) m_altitude = altitude;
}
void WaypointPilot::set_altitude(float altitude) {
	m_altitude = altitude;
}

}  // namespace autopilot

