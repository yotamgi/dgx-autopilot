#include "plain.h"
#include <iostream>
#include <boost/foreach.hpp>

namespace simulator {

typedef irr::core::vector3df irrvec3f;

static irrvec3f to_irr_vec(const lin_algebra::vec3f& vec) {
	return irrvec3f(vec[0], vec[1], vec[2]);
}

PlainParams::PlainParams(const std::string& mesh_file,
		const std::string& texture_file,
		const irr::core::vector3df scale,
		const irr::core::vector3df rot,
		float yaw_speed,					// degrees/sec
		float pitch_speed,					// degrees/sec
		float ailron_speed,					// degrees/sec
		float mass,
		float engine_power,
		float drag,
		float wings_area,
		float wings_lift,
		float sideslide_yaw_effect_strenth,
		float dihedral_effect_strenth):
	m_mesh_file(mesh_file),
	m_texture_file(texture_file),
	m_scale(scale),
	m_rot(rot),
	m_rudder_intensity(yaw_speed),
	m_elevator_intensity(pitch_speed),
	m_ailron_intensity(ailron_speed),
	m_mass(mass),
    m_engine_power(engine_power),
	m_drag(drag),
	m_wings_area(wings_area),
	m_wings_lift(wings_lift),
	m_sideslide_yaw_effect_strenth(sideslide_yaw_effect_strenth),
	m_dihedral_effect_strenth(dihedral_effect_strenth)
{}

Plain::Plain(irr::IrrlichtDevice* device,
		irrvec3f start_pos,
		boost::shared_ptr<WindGen> wind_gen,
		const PlainParams plane_params):
		FlyingObject(device),
		m_ailron_servo(new Servo(130.0, 50.)),
		m_elevator_servo(new Servo(130.0, 50.)),
		m_rudder_servo(new Servo(130.0, 50.)),
		m_throttle_servo(new Servo(80.0, 0.)),
		m_velocity(irrvec3f(0.0f, 0.0f, 0.0f)),
		m_acceleration(irrvec3f(0.0f, 0.0f, 0.0f)),
		m_params(plane_params),
		m_print_timer(0.),
		m_data_ready(false),
		m_wind_gen(wind_gen)
{
	irr::scene::ISceneManager* smgr = m_device->getSceneManager();

	m_object = smgr->addMeshSceneNode(smgr->getMesh(m_params.get_mesh_file().c_str()));
	if (!m_object) {
		throw std::runtime_error("Could not load the plane mesh");
	}

	m_object->setPosition(start_pos);
	m_object->setScale(plane_params.get_scale());
	m_object->setRotation(plane_params.get_rot());
	m_object->setMaterialTexture(0, m_device->getVideoDriver()->getTexture(m_params.get_texture_file().c_str()));
	m_object->setMaterialFlag(irr::video::EMF_LIGHTING, false);

	m_transformation.setRotationDegrees(irrvec3f(lin_algebra::frand()/10.,
						lin_algebra::frand()/10., lin_algebra::frand()/10)); // it cant be actual zeors
}

irrvec3f Plain::calc_angle_vel(float time_delta) {

	irrvec3f airspeed = m_velocity - to_irr_vec(m_wind_gen->get_wind());

	float pitch_data = m_elevator_servo->get_data(time_delta);
	float tilt_data  = m_ailron_servo->get_data(time_delta);
	float yaw_data = m_rudder_servo->get_data(time_delta);

	float vel_len = airspeed.getLength();
	irrvec3f servo_data(
				 ((pitch_data - 50.)/50.)*m_params.get_elevator_intensity(),
				-((yaw_data   - 50.)/50.)*m_params.get_rudder_intensity(),
				 ((tilt_data  - 50.)/50.)*m_params.get_ailron_intensity()
	);
	irrvec3f angle_vel = (vel_len/100.)*servo_data;

	// calc the plane's side slide
	irr::core::matrix4 inverse_trans;
	m_transformation.getInverse(inverse_trans);
	irrvec3f velocity_plane = airspeed;
	inverse_trans.rotateVect(velocity_plane);
	velocity_plane.normalize();
	velocity_plane *= vel_len;
	float side_slide = velocity_plane.X;

	// sideslide to yaw effect
	angle_vel.Y += side_slide*m_params.get_sideslide_yaw_effect_strenth();

	// dihedral effect
	angle_vel.Z += side_slide*m_params.get_dihedral_effect_strenth();

	return angle_vel;
}

irrvec3f Plain::calc_plane_acceleration(float time_delta) {
	// TODO : the calc_angle_vel should consieder the attack
	const float ATTACK_DRAG_AFFECTION = 1.2;
	const float AIR_DENSITY = 1.293;
	const float MIN_LIFT_ATTACK_ANGLE = irr::core::degToRad(m_params.get_wings_lift());

	// calculate some useful params
	irrvec3f airspeed = m_velocity - to_irr_vec(m_wind_gen->get_wind());
	irrvec3f plane_heading = irrvec3f(0., 0., 1.);
	m_transformation.rotateVect(plane_heading);
	plane_heading.normalize();
	irrvec3f plane_up = irrvec3f(0., 1., 0.);
	m_transformation.rotateVect(plane_up);
	plane_up.normalize();
	const float vel_length = airspeed.getLength();
	irrvec3f vel_dir = airspeed;
	vel_dir.normalize();

	// calculate the attack angle
	irr::core::matrix4 inverse_trans;
	m_transformation.getInverse(inverse_trans);
	irrvec3f velocity_plane = vel_dir;
	inverse_trans.rotateVect(velocity_plane);
	velocity_plane.X = 0.;
	velocity_plane.normalize();
	float attack_angle = acos( velocity_plane.dotProduct(irrvec3f(0., 0., 1.)) );
	attack_angle *= lin_algebra::sign(-1. * velocity_plane.Y);

	// calculate the engine force
	irrvec3f engine_force = plane_heading;
	engine_force *= (m_throttle_servo->get_data(time_delta)/100.) * m_params.get_engine_power();

	// calculate the drag force
	irrvec3f drag_force = -1. * vel_dir;
	drag_force *= 0.5 * AIR_DENSITY * vel_length*vel_length * m_params.get_drag();
	drag_force *= ATTACK_DRAG_AFFECTION * (11.5*fabs(attack_angle) + MIN_LIFT_ATTACK_ANGLE / (-0.085));

	// calculate the lift force
	irrvec3f lift_force = plane_up;
	lift_force *= 0.5 * AIR_DENSITY * vel_length*vel_length * m_params.get_wings_area();
	lift_force *= 11.5*attack_angle + MIN_LIFT_ATTACK_ANGLE / (-0.085);

	// calculate the gravity force
	irrvec3f gravity_force = irrvec3f(0., -1., 0.) * m_params.get_mass() * 10.;

	// calculate the acceleration
	irrvec3f acc = (engine_force + drag_force + lift_force + gravity_force) / m_params.get_mass();

	if (m_print_timer > 0.1) {
		m_print_timer = 0.;
		std::cout << std::setprecision(2) << std::fixed <<
				"Plane's alt is " << m_object->getPosition().Y <<
				" and speed is " << vel_length*3.6 <<
				" km/h attack:" << irr::core::radToDeg(attack_angle) << std::endl;
	}

	return acc;
}

void Plain::update(float time_delta) {
	irrvec3f pos = m_object->getPosition();

	irrvec3f angle_vel = calc_angle_vel(time_delta);
	irrvec3f angle_diff = angle_vel * time_delta;
	irr::core::matrix4 rot_mat;
	rot_mat.setRotationDegrees(angle_diff);
	m_transformation *= rot_mat;

	// update the velocity by the acceleration
	m_velocity += m_acceleration * time_delta;

	// update the position by the velocity
	pos  += m_velocity * time_delta;

	// update acceleration
	m_acceleration = calc_plane_acceleration(time_delta);

	// make sure we are not on the ground
	if (pos.Y < 0.) {
		pos.Y = 0;
		m_velocity.Y = 0;
	}

	// Update the object with the new data
	irr::core::matrix4 plain_rot;
	plain_rot.setRotationDegrees(m_params.get_rot());
	irr::core::matrix4 final_trans = m_transformation * plain_rot;
	m_object->setRotation(final_trans.getRotationDegrees());
	m_object->setPosition(pos);

	m_print_timer += time_delta;

	BOOST_FOREACH(boost::shared_ptr<Carriable> carried, m_stuff) {
		carried->update(time_delta);
	}

	m_data_ready = true;

}

void Plain::carry(boost::shared_ptr<simulator::Carriable> carried)  {
	carried->setSensedObject(m_object);
	m_stuff.push_back(carried);
}

Plain::~Plain() {
	BOOST_FOREACH(boost::shared_ptr<Carriable> carried, m_stuff) {
		carried->setSensedObject(NULL);
	}
}

Plain::Servo::Servo(float speed, float initial_val):
		m_speed(speed),
		m_state(initial_val),
		m_override(false),
		m_override_target(0.),
		m_target(initial_val)
{}

void Plain::Servo::set_data(const float& data) {
	if (data > 100.) 	m_target = 100.;
	if (data < 0.)		m_target = 0.;
	else m_target = data;
}

void Plain::Servo::override(const float& data) {
	m_override = true;

	if (data > 100.) 	m_override_target = 100.;
	if (data < 0.)		m_override_target = 0.;
	else m_override_target = data;
}

void Plain::Servo::stop_override() {
	m_override = false;
}

float Plain::Servo::get_data(float time_delta) {

	float target = m_override? m_override_target:m_target;

	float distance = target - m_state;

	// calc the needed diff
	float new_state;
	if (fabs(distance) > 1.0) {
		new_state = m_state + lin_algebra::sign(distance) * time_delta*m_speed;

		if (lin_algebra::sign(target - m_state) != lin_algebra::sign(target - new_state)) {
			m_state = target;
		} else {
			m_state = new_state;
		}
	}

	return m_state;
}


}  // namespace simulator

