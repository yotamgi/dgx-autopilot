#include "plane.h"
#include <iostream>

namespace simulator {

/**
 * Retunrs random float between -0.5 to 0.5
 */
static float frand() {
	return (float)rand()/(float)RAND_MAX - 0.5;
}

PlainParams::PlainParams(const std::string& mesh_file,
		const std::string& texture_file,
		const irr::core::vector3df scale,
		float speed,
		float yaw_speed,
		float pitch_speed,
		float ailron_speed):
	m_mesh_file(mesh_file),
	m_texture_file(texture_file),
	m_scale(scale),
	m_speed(speed),
	m_yaw_speed(yaw_speed),
	m_pitch_speed(pitch_speed),
	m_ailron_speed(ailron_speed)
{}

Plane::Plane(irr::IrrlichtDevice* device,
		irr::core::vector3df start_pos,
		const PlainParams plane_params):
		FlyingObject(device),
		m_gyro(new SensorGenerator),
		m_acc(new SensorGenerator),
		m_compass(new SensorGenerator),
		m_direction(irr::core::vector3df(0.0f, 0.0f, -1.0f)),
		m_params(plane_params),
		m_gyro_drift(frand()*10., frand()*10., frand()*10.)
{
	m_servos.yaw_percentage = 50.0f;
	m_servos.pitch_percentage = 50.0f;
	m_servos.ailron_percentage = 50.0f;

	irr::scene::ISceneManager* smgr = m_device->getSceneManager();

	m_object = smgr->addMeshSceneNode(smgr->getMesh(m_params.get_mesh_file().c_str()));
	if (!m_object) {
		throw std::runtime_error("Could not load the plane mesh");
	}

	m_object->setPosition(start_pos);
	m_object->setScale(plane_params.get_scale());
	m_object->setMaterialTexture(0, m_device->getVideoDriver()->getTexture(m_params.get_texture_file().c_str()));
	m_object->setMaterialFlag(irr::video::EMF_LIGHTING, false);
}

irr::core::vector3df Plane::calc_angle_vel() const {
	return irr::core::vector3df(
				((m_servos.pitch_percentage   - 50.0f)/50.0f)*m_params.get_pitch_speed(),
				((m_servos.yaw_percentage 	  - 50.0f)/50.0f)*m_params.get_yaw_speed(),
				((m_servos.ailron_percentage  - 50.0f)/50.0f)*m_params.get_ailron_speed()
	);
}


void Plane::update(float time_delta) {
	irr::core::vector3df pos = m_object->getPosition();
	irr::core::vector3df dir = m_direction;

	// update the the rotation by the servos values
	irr::core::matrix4 transformation = m_object->getAbsoluteTransformation();

	irr::core::vector3df angle_vel = calc_angle_vel();
	irr::core::vector3df angle_diff = angle_vel *time_delta;
	irr::core::matrix4 rot_mat;
	rot_mat.setRotationDegrees(angle_diff);
	transformation *= rot_mat;

	// update the direction by the rotation values
	transformation.rotateVect(dir);
	dir.normalize();

	// update the position by the dirction value
	pos  += dir*time_delta*m_params.get_speed();

	// Update the object with the new data
	m_object->setRotation(transformation.getRotationDegrees());
	m_object->setPosition(pos);

	update_sensors(time_delta);
}

void Plane::update_sensors(float time_delta) {

	// calculate some data
	irr::core::vector3df angle_vel = calc_angle_vel();
	irr::core::matrix4 trans = m_object->getAbsoluteTransformation();
	irr::core::vector3df dir;
	trans.rotateVect(dir, m_direction);

	// update the gyro
	SensorGenerator::vector_t gyro_data;
	gyro_data[0] = angle_vel.X + frand()*10. + m_gyro_drift.X;
	gyro_data[1] = angle_vel.Y + frand()*10. + m_gyro_drift.Y;
	gyro_data[2] = angle_vel.Z + frand()*10. + m_gyro_drift.Z;
	m_gyro->set_data(gyro_data);

	// update the accelerometer
	SensorGenerator::vector_t acc_data;
	irr::core::vector3df g(0, -1., 0);
	irr::core::vector3df acc = g + 4.*(m_priv_dir - dir)/time_delta;
	float acc_len = acc.getLength();
	trans.rotateVect(acc);
	acc = acc.normalize()*acc_len; // rotateVect doesn't maintain vec size...
	acc_data[0] = acc.X + frand()*0.05;
	acc_data[1] = acc.Y + frand()*0.05;
	acc_data[2] = acc.Z + frand()*0.05;
	m_acc->set_data(acc_data);

	// update the compass
	SensorGenerator::vector_t compass_data;
	irr::core::vector3df north(1., -1., 0);
	trans.rotateVect(north);
	north = north.normalize() * 20.;
	compass_data[0] = north.X;
	compass_data[1] = north.Y;
	compass_data[2] = north.Z;
	m_compass->set_data(compass_data);

	m_priv_dir = dir;

}

void Plane::set_pitch_servo(float percentage) {
	m_servos.pitch_percentage = percentage;
}
void Plane::set_yaw_servo(float percentage) {
	m_servos.yaw_percentage = percentage;
}
void Plane::set_ailron_servo(float percentage) {
	m_servos.ailron_percentage = percentage;
}


}  // namespace simulator

