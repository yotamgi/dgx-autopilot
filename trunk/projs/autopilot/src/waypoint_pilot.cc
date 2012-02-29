#include "waypoint_pilot.h"
#include <cmath>
#include <iostream>

namespace autopilot {

WaypointPilot::waypoint::waypoint(lin_algebra::vec2f target_, float altitude_):
		target(target_),
		altitude(altitude_)
{}

WaypointPilot::WaypointPilot(const WaypointPilotParams& params, boost::shared_ptr<NormalPlainCockpit> cockpit):
		m_cockpit(cockpit),
		m_waiting_path(false),
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

void WaypointPilot::maintain_pitch(float wanted_pitch) {
	float plain_pitch = m_cockpit->orientation()->get_data()[0];
	float pitch_delta = wanted_pitch - plain_pitch;
	m_cockpit->pitch_servo()->set_data(50. - 50.*(pitch_delta)/m_params.max_climbing_angle);
}

void WaypointPilot::maintain_alt(float wanted_altitude){

	servo_stream_ptr gas_servo = m_cockpit->gas_servo();

	float delta_alt = wanted_altitude - m_cockpit->alt()->get_data();

	// max climbing
	if (delta_alt > 5) {
		gas_servo->set_data(m_params.climbing_gas);
		maintain_pitch(m_params.max_climbing_angle);
	}

	// max decending
	else if (delta_alt < -5) {
		gas_servo->set_data(m_params.decending_gas);
		maintain_pitch(m_params.max_decending_angle);
	}

	// avg flight
	else {
		gas_servo->set_data(m_params.avg_gas);
		maintain_pitch(m_params.avg_pitch);
	}
}

void WaypointPilot::maintain_angle(float angle){

	vec3_stream_ptr oreintation = m_cockpit->orientation();
	servo_stream_ptr tilt = m_cockpit->tilt_servo();

	lin_algebra::vec3f plain_tilt = oreintation->get_data();

	if(plain_tilt[2] > m_params.max_tilt_angle) {
		tilt->set_data(100.);
	} else if (plain_tilt[2] < -1.*m_params.max_tilt_angle) {
		tilt->set_data(0.);
	} else {
		tilt->set_data(50. - 50.*(angle - plain_tilt[2])/m_params.max_tilt_angle);
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

	if (delta_heading > 5)
		maintain_angle(-16);
	else if (delta_heading < -5)
		maintain_angle(16);
	else
		maintain_angle(-delta_heading/2);
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

	 // calc the distance from the waypoint
	 float distance = lin_algebra::vec_len(wanted_direction);

	 if (distance < 10)
		 return true;

	 else return false;
 }


void WaypointPilot::fly() {

	while (m_running) {

		// update the path synchronusly, to avoid thread safty problems
		if (m_waiting_path) {
			m_path = m_new_path;
			m_waiting_path = false;
		}

		// fly the plane
		bool there_yet = nav_to(m_path.at(0));

		// if we got to the point, shrink the vector
		if (there_yet) {
			m_path.erase(m_path.begin());
		}
	}
}

void WaypointPilot::set_path(WaypointPilot::waypoint_list path) {
	while (m_waiting_path);

	m_new_path = path;
	m_waiting_path = true;
}

}  // namespace autopilot

