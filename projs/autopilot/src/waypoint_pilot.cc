#include "waypoint_pilot.h"
#include <stream/util/time.h>
#include <cmath>
#include <iostream>

namespace autopilot {

WaypointPilot::waypoint::waypoint(lin_algebra::vec2f target_, float altitude_):
		target(target_),
		altitude(altitude_)
{}

WaypointPilot::WaypointPilot(const Params& params, boost::shared_ptr<NormalPlainCockpit> cockpit):
		m_cockpit(cockpit),
		m_sas_pilot(cockpit, params.pitch_severity, params.roll_severity),
		m_waiting_path(false),
		m_running(false),
		m_airspeed_low_pass(-1.),
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
		m_sas_pilot.get_tilt_control()->set_data(m_params.max_climbing_angle);
//		gas_servo->set_data(m_params.climbing_gas);
	}

	// max decending
	else if (delta_alt < -5) {
//		gas_servo->set_data(m_params.decending_gas);
		m_sas_pilot.get_tilt_control()->set_data(m_params.max_decending_angle);
	}

	// avg flight
	else {
//		gas_servo->set_data(m_params.avg_gas);
		if (delta_alt > 0.) {
			m_sas_pilot.get_tilt_control()->set_data((delta_alt/5.)*m_params.max_climbing_angle);
		} else {
			m_sas_pilot.get_tilt_control()->set_data(-(delta_alt/5.)*m_params.max_decending_angle);
		}
	}
}

void WaypointPilot::maintain_airspeed(float airspeed) {

	float wanted_gas;

	if (m_params.use_airspeed) {

		if (m_airspeed_low_pass < 0) m_airspeed_low_pass = m_cockpit->air_speed()->get_data();

		// make the airpseed low-pass filter
		m_airspeed_low_pass += 0.7 * (m_cockpit->air_speed()->get_data() - m_airspeed_low_pass) * m_t.passed();
		m_t.reset();

		// set gas according to the wanted airspeed
		float airspeed_delta = m_params.avg_airspeed - m_airspeed_low_pass;
		wanted_gas = 50. + 50 * (airspeed_delta / m_params.airspeed_severity);
	} else {

		float pitch_angle = m_cockpit->orientation()->get_data()[0];

		if (pitch_angle > 0.75*m_params.max_climbing_angle) {
			wanted_gas = m_params.climbing_gas;
		} else if (pitch_angle < 0.75*m_params.max_climbing_angle) {
			wanted_gas = m_params.avg_gas;
		} else {
			wanted_gas = m_params.decending_gas;
		}
	}

	m_cockpit->gas_servo()->set_data(wanted_gas);
}

void WaypointPilot::maintain_heading(float heading) {

	vec3_stream_ptr oreintation = m_cockpit->orientation();
	vec2_stream_ptr position = m_cockpit->position();

	lin_algebra::vec3f plain_oreintation = oreintation->get_data();

	// calc the smaller angle  180 - -180
	float delta_heading = plain_oreintation[1] - heading;
	if (delta_heading > 180)
		delta_heading = delta_heading - 360.;
	else if (delta_heading < -180 )
		delta_heading = 360 + delta_heading;

	if (delta_heading > m_params.heading_fine_tunning)
		m_sas_pilot.get_roll_control()->set_data(-m_params.max_roll_angle);
	else if (delta_heading < -m_params.heading_fine_tunning)
		m_sas_pilot.get_roll_control()->set_data(m_params.max_roll_angle);
	else
		m_sas_pilot.get_roll_control()->set_data(-m_params.max_roll_angle*delta_heading/m_params.heading_fine_tunning);

}


 bool WaypointPilot::nav_to(WaypointPilot::waypoint waypoint) {

	 vec2_stream_ptr position = m_cockpit->position();
	 lin_algebra::vec2f plain_pos = position->get_data();
	 lin_algebra::vec2f wanted_direction = waypoint.target - plain_pos;

	 // calc the wanted angle
	 float heading = 180.*std::atan(wanted_direction[0]/wanted_direction[1])/lin_algebra::PI;
	 if (wanted_direction[1] < 0.) {
		 heading += 180;
	 }

	 maintain_heading(heading);

	 maintain_alt(waypoint.altitude);

	 maintain_airspeed(m_params.avg_airspeed);

	 m_sas_pilot.update();

	 // calc the distance from the waypoint
	 float distance = lin_algebra::vec_len(wanted_direction);

	 if (distance < 10)
		 return true;

	 else return false;
 }


void WaypointPilot::fly() {

	while (m_running) {

		// update the cockpit first, and update it twice because it is
		// more important
		m_cockpit->update();
		m_cockpit->update();

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

