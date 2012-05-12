#include "waypoint_pilot.h"
#include <cmath>
#include <iostream>

namespace autopilot {

WaypointPilot::waypoint::waypoint(lin_algebra::vec2f target_, float altitude_):
		target(target_),
		altitude(altitude_)
{}

WaypointPilot::WaypointPilot(const Params& params, boost::shared_ptr<NormalPlainCockpit> cockpit):
		m_cockpit(cockpit),
		m_sas_pilot(params, cockpit),
		m_waiting_path(false),
		m_running(false),
		m_params(params)
{
	m_roam_waypoint = waypoint(cockpit->position()->get_data(), 100);
}

void WaypointPilot::start(bool open_thread) {
	if (m_running) return;
	if (open_thread) {
		m_running_thread = boost::thread(&WaypointPilot::start, this, false);
	} else {
		m_running = true;
		fly();
	}
}

void WaypointPilot::stop() {
	if (!m_running) return;
	m_running = false;
	m_running_thread.join();
}

void WaypointPilot::maintain_alt(float wanted_altitude){

	servo_stream_ptr gas_servo = m_cockpit->gas_servo();

	float delta_alt = wanted_altitude - m_cockpit->alt()->get_data();

	// max climbing
	if (delta_alt > 5) {
		gas_servo->set_data(m_params.climbing_gas);
		m_sas_pilot.get_tilt_control()->set_data(m_params.max_climbing_strength);
	}

	// max decending
	else if (delta_alt < -5) {
		gas_servo->set_data(m_params.decending_gas);
		m_sas_pilot.get_tilt_control()->set_data(m_params.max_decending_strength);
	}

	// avg flight
	else {
		gas_servo->set_data(m_params.avg_gas);
		m_sas_pilot.get_tilt_control()->set_data(m_params.avg_pitch_strength);
	}
}

void WaypointPilot::maintain_heading(float heading){

	vec3_stream_ptr oreintation = m_cockpit->orientation();
	vec2_stream_ptr position = m_cockpit->position();

	lin_algebra::vec3f plain_oreintation = oreintation->get_data();

	// calc the smaller angle  180 - -180
	float delta_heading = plain_oreintation[1] - heading;
	if (delta_heading > 180)
		delta_heading = delta_heading - 360.;
	else if (delta_heading < -180 )
		delta_heading = 360 + delta_heading;

	if (delta_heading > 10.0f)
		m_sas_pilot.get_roll_control()->set_data(0.0f);
	else if (delta_heading < -10.0f)
		m_sas_pilot.get_roll_control()->set_data(100.0f);
	else
		m_sas_pilot.get_roll_control()->set_data(50.0f - 25.0f*delta_heading/10.0f);

}


 bool WaypointPilot::nav_to(WaypointPilot::waypoint waypoint) {

	 vec2_stream_ptr position = m_cockpit->position();
	 lin_algebra::vec2f plain_pos = position->get_data();
	 lin_algebra::vec2f wanted_direction = plain_pos - waypoint.target;

	 // calc the wanted angle
	 float heading = 180.*std::atan(wanted_direction[0]/wanted_direction[1])/lin_algebra::PI;
	 if (wanted_direction[1] < 0.) {
		 heading += 180;
	 }

	 maintain_heading(heading);

	 maintain_alt(waypoint.altitude);

	 m_sas_pilot.update();

	 // calc the distance from the waypoint
	 float distance = lin_algebra::vec_len(wanted_direction);

	 if (distance < 10)
		 return true;

	 else return false;
 }


void WaypointPilot::fly() {

	while (m_running) {

		// update the path synchronously, to avoid thread safety problems
		if (m_waiting_path) {
			m_path = m_new_path;
			m_waiting_path = false;
			std::cout << "Got new path, which contains " << m_path.size() << " waypoints" << std::endl;
		}

		if (m_path.size() != 0) {

			// fly the plane
			bool there_yet = nav_to(m_path.at(0));

			// if we got to the point, shrink the vector
			if (there_yet) {
				std::cout << "Got there!" << std::endl;

				if (m_path.size() == 1) {
					std::cout << "Finished all waypoints - just roam..." << std::endl;
					m_roam_waypoint = m_path.at(0);
				}

				m_path.erase(m_path.begin());
			}
		}

		else {
			// roam
			nav_to(m_roam_waypoint);
		}

	}
}

void WaypointPilot::set_path(WaypointPilot::waypoint_list path) {
	while (m_waiting_path);

	m_new_path = path;
	m_waiting_path = true;
}

}  // namespace autopilot

