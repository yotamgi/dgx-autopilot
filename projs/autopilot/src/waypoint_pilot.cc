#include "waypoint_pilot.h"
#include <cmath>
#include <iostream>

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

void WaypointPilot::maintain_pitch(float pitch_a){

	vec3_stream_ptr oreintation = m_cockpit->orientation();
	servo_stream_ptr pitch = m_cockpit->pitch_servo();


	lin_algebra::vec3f plain_pitch = oreintation->get_data();


	if(plain_pitch[0] > m_params.max_climbing_angle)
				pitch->set_data(100.);
	else if (plain_pitch[0] <  -1.*m_params.max_climbing_angle)
				pitch->set_data(0.);
	else
				pitch->set_data(50. - 50.*(pitch_a - plain_pitch[0])/m_params.max_climbing_angle);


	//return plain_pitch[0];
}


void WaypointPilot::maintain_angle(float angle){

	vec3_stream_ptr oreintation = m_cockpit->orientation();
	servo_stream_ptr tilt = m_cockpit->tilt_servo();


	lin_algebra::vec3f plain_tilt = oreintation->get_data();

	if(plain_tilt[2] > m_params.max_tilt_angle)
				tilt->set_data(100.);
	else if (plain_tilt[2] < -1.*m_params.max_tilt_angle)
				tilt->set_data(0.);
	else
				tilt->set_data(50. - 50.*(angle - plain_tilt[2])/m_params.max_tilt_angle);


}

void WaypointPilot::maintain_heading(float heading){

	vec3_stream_ptr oreintation = m_cockpit->orientation();
	vec2_stream_ptr position = m_cockpit->position();


	lin_algebra::vec3f plain_oreintation = oreintation->get_data();

	//calc the smaller angle  180 - -180
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



 void WaypointPilot::maintain_alt(float altitude){

	vec3_stream_ptr oreintation = m_cockpit->orientation();
	vec2_stream_ptr position = m_cockpit->position();
	float_stream_ptr alt = m_cockpit->alt();
	float_stream_ptr speed = m_cockpit->ground_speed();
	servo_stream_ptr gas = m_cockpit->gas_servo();

	float delta_alt;

	delta_alt = alt->get_data()*10. - altitude;

	if(delta_alt < -5){				//max climbing
		gas->set_data(100.);
		maintain_pitch(4.);
	}
	else if (delta_alt > 5){		//max
		gas->set_data(0.);
		maintain_pitch(-10);
	}
	else {							//need to be air speed loop
		gas->set_data(28.);
		maintain_pitch(0.);
	}
 }


 bool WaypointPilot::nav_to(lin_algebra::vec2f waypoint, float alt){


	 vec2_stream_ptr position = m_cockpit->position();
	 lin_algebra::vec2f plain_pos = position->get_data();
	 float distance;
	 lin_algebra::vec2f wanted_direction =   plain_pos - waypoint;

	 //calc the wanted angle
	 float heading = 180.*std::atan(wanted_direction[0]/wanted_direction[1])/lin_algebra::PI;


	 //fix the tag answer
	 if (wanted_direction[1] < 0.)
		 heading += 180;

	 maintain_heading(heading);

	 maintain_alt(alt);

	 //calc the distance from the waypoint
	 distance = lin_algebra::vec_len(wanted_direction);

	 if (distance < 10)
		 return true;
	 else
	 return false;



 }


void WaypointPilot::fly() {




	while (m_running) {





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

