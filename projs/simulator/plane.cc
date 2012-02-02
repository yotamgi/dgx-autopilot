#include "plane.h"
#include <iostream>

namespace simulator {

typedef irr::core::vector3df irrvec3f;

/**
 * Retunrs random float between -0.5 to 0.5
 */
static float frand() {
	return (float)rand()/(float)RAND_MAX - 0.5;
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
		float wings_lift):
	m_mesh_file(mesh_file),
	m_texture_file(texture_file),
	m_scale(scale),
	m_rot(rot),
	m_yaw_speed(yaw_speed),
	m_pitch_speed(pitch_speed),
	m_ailron_speed(ailron_speed),
	m_mass(mass),
    m_engine_power(engine_power),
	m_drag(drag),
	m_wings_area(wings_area),
	m_wings_lift(wings_lift)
{}

Plane::Plane(irr::IrrlichtDevice* device,
		irrvec3f start_pos,
		const PlainParams plane_params):
		FlyingObject(device),
		m_gyro(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f())),
		m_acc(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f())),
		m_compass(new stream::PushToPopConv<lin_algebra::vec3f>(lin_algebra::vec3f())),
		m_tilt_servo(new stream::PushToPopConv<float>(50.)),
		m_pitch_servo(new stream::PushToPopConv<float>(50.)),
		m_yaw_servo(new stream::PushToPopConv<float>(50.)),
		m_gas_servo(new stream::PushToPopConv<float>(0.)),
		m_velocity(irrvec3f(0.0f, 0.0f, 0.0f)),
		m_params(plane_params),
		m_gyro_drift(frand()*1., frand()*1., frand()*1.),
		m_gps_thread(&Plane::gps_update, this),
		m_forced_tilt(-1.),
		m_forced_pitch(-1.),
		m_print_timer(0.),
		m_past_samples(m_avarge_len),
		m_data_ready(false)
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

	m_transformation.setRotationDegrees(irrvec3f(frand()/10., frand()/10., frand()/10)); // it cant be actual zeors
}

irrvec3f Plane::calc_angle_vel() const {

	float pitch_data = m_forced_pitch > 0 ? m_forced_pitch : m_pitch_servo->get_data();
	float tilt_data  = m_forced_tilt  > 0 ? m_forced_tilt  : m_tilt_servo->get_data();
	float yaw_data = m_yaw_servo->get_data();

	irrvec3f servo_angle_speed(
				((pitch_data - 50.)/50.)*m_params.get_pitch_speed(),
				((yaw_data   - 50.)/50.)*m_params.get_yaw_speed(),
				((tilt_data  - 50.)/50.)*m_params.get_ailron_speed()
	);

	irr::core::matrix4 inverse_trans;
	m_transformation.getInverse(inverse_trans);
	irrvec3f velocity_plane = m_velocity;
	inverse_trans.rotateVect(velocity_plane);
	velocity_plane.normalize();
	servo_angle_speed.Y += velocity_plane.X*1000.;

	return servo_angle_speed;
}

irrvec3f Plane::calc_plane_acceleration() {
	// TODO : the calc_angle_vel should consieder the attack
	const float ATTACK_DRAG_AFFECTION = 1.2;
	const float AIR_DENSITY = 1.293;
	const float MIN_LIFT_ATTACK_ANGLE = irr::core::degToRad(m_params.get_wings_lift());

	// calculate some useful params
	irrvec3f plane_heading = irrvec3f(0., 0., 1.);
	m_transformation.rotateVect(plane_heading);
	plane_heading.normalize();
	irrvec3f plane_up = irrvec3f(0., 1., 0.);
	m_transformation.rotateVect(plane_up);
	plane_up.normalize();
	const float vel_length = m_velocity.getLength();
	irrvec3f vel_dir = m_velocity;
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
	engine_force *= (m_gas_servo->get_data()/100.) * m_params.get_engine_power();

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


	if (m_print_timer > 1.) {
		m_print_timer = 0.;
		std::cout << std::setprecision(2) << std::fixed <<
				"Plane's alt is " << m_object->getPosition().Y <<
				" and speed is " << vel_length*3.6 <<
				" km/h attack:" << irr::core::radToDeg(attack_angle) << std::endl;
	}

	return acc;
}

void Plane::update(float time_delta) {
	irrvec3f pos = m_object->getPosition();

	irrvec3f angle_vel = calc_angle_vel();
	irrvec3f angle_diff = angle_vel * time_delta;
	irr::core::matrix4 rot_mat;
	rot_mat.setRotationDegrees(angle_diff);
	m_transformation *= rot_mat;

	// update the speed by the acceleration
	irrvec3f acceleration = calc_plane_acceleration();
	m_velocity += acceleration * time_delta;

	// update the position by the velocity
	pos  += m_velocity * time_delta;

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
	update_sensors(time_delta);
}

void Plane::update_sensors(float time_delta) {
	if (time_delta == 0.0f) {
		time_delta = 0.001f;
	}

	// calculate some data
	irrvec3f angle_vel = calc_angle_vel();
	irrvec3f acceleration = (m_velocity - m_priv_vel)/time_delta;

	// update the gyro
	lin_algebra::vec3f gyro_data;
	gyro_data[0] = angle_vel.X + frand()*2.0 + 2*m_gyro_drift.X;
	gyro_data[1] = angle_vel.Y + frand()*2.0 + 2*m_gyro_drift.Y;
	gyro_data[2] = angle_vel.Z + frand()*2.0 + 2*m_gyro_drift.Z;
	m_gyro->set_data(gyro_data);

	// update the accelerometer
	lin_algebra::vec3f acc_data;
	irrvec3f g(0, -1., 0);
	irrvec3f acc = g - 0.1*acceleration;
	float acc_len = acc.getLength();
	m_transformation.getTransposed().rotateVect(acc);
	acc = acc.normalize()*acc_len; // rotateVect doesn't maintain vec size...
	acc_data[0] = acc.X + frand()*0.05;
	acc_data[1] = acc.Y + frand()*0.05;
	acc_data[2] = acc.Z + frand()*0.05;
	m_acc->set_data(acc_data);

	// update the compass
	lin_algebra::vec3f compass_data;
	irrvec3f north(1., -1., 0);
	m_transformation.getTransposed().rotateVect(north);
	north = north.normalize() * 20.;
	compass_data[0] = north.X;
	compass_data[1] = north.Y;
	compass_data[2] = north.Z;
	m_compass->set_data(compass_data);

	m_priv_vel = m_velocity;

	m_data_ready = true;
}

void Plane::gps_update() {

	while (true) {
		if (!m_data_ready) continue;
		// calculate the position
		irrvec3f irrpos = m_object->getPosition();
		lin_algebra::vec3f gps_pos;
		gps_pos[0] = irrpos.X;
		gps_pos[1] = irrpos.Y;
		gps_pos[2] = irrpos.Z;

		lin_algebra::vec3f rand;
		rand.randu();
		rand = lin_algebra::normalize(rand);
		gps_pos += rand;

		// calculate the speed
		m_past_samples.push_back(gps_pos);

		// fill the data into a matrix
		lin_algebra::Mat<float> m(2, m_past_samples.size());
		for (size_t i=0; i<m_past_samples.size(); i++) {
			m.col(i) = lin_algebra::get<2, 0>(m_past_samples.at(i));
		}
		m = lin_algebra::trans(m);

		lin_algebra::Mat<float> ce, score;
		lin_algebra::princomp(ce, score, m);
		lin_algebra::vec2f speed = ce.col(0) * score.at(0,0) * -2.;
		//float reliability = score.at(0, 1);
		float speed_mag = std::sqrt(speed[0]*speed[0] + speed[1]*speed[1]);
		float speed_angle = std::atan(speed[1]/speed[0])/lin_algebra::PI * 180.;

		if (m_gps_pos_listener) {
			m_gps_pos_listener->set_data(gps_pos);
		}
		if (m_gps_speed_dir_listener) {
			m_gps_speed_dir_listener->set_data(speed_angle);
		}
		if (m_gps_speed_mag_listener) {
			m_gps_speed_mag_listener->set_data(speed_mag);
		}

		sleep(1);
	}
}

void Plane::set_gps_pos_listener(boost::shared_ptr<stream::DataPushStream<lin_algebra::vec3f> > listenr) {
	m_gps_pos_listener = listenr;
}

void Plane::set_gps_speed_dir_listener(boost::shared_ptr<stream::DataPushStream<float> > listenr) {
	m_gps_speed_dir_listener = listenr;
}
void Plane::set_gps_speed_mag_listener(boost::shared_ptr<stream::DataPushStream<float> > listenr) {
	m_gps_speed_mag_listener = listenr;
}

}  // namespace simulator

