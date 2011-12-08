#include "waypoint_pilot.h"

namespace autopilot {

WaypointPilot::WaypointPilot(boost::shared_ptr<Cockpit> cockpit):
		m_cockpit(cockpit),
		m_running(false)
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
	servo_stream_ptr tilt = m_cockpit->tilt_servo();
	servo_stream_ptr pitch = m_cockpit->pitch_servo();

	while (m_running) {
		m_cockpit->pitch_servo()->set_data(70.);
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

