#include "plane.h"

namespace simulator {


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
		m_direction(irr::core::vector3df(0.0f, 0.0f, -1.0f)),
		m_params(plane_params)
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

irr::core::vector3df Plane::calc_angle_diff(float time_delta) const {
	return irr::core::vector3df(
				((m_servos.pitch_percentage   - 50.0f)/50.0f)*time_delta*m_params.get_pitch_speed(),
				((m_servos.yaw_percentage 	  - 50.0f)/50.0f)*time_delta*m_params.get_yaw_speed(),
				((m_servos.ailron_percentage  - 50.0f)/50.0f)*time_delta*m_params.get_ailron_speed()
	);
}


void Plane::update(float time_delta) {
	irr::core::vector3df pos = m_object->getPosition();
	irr::core::vector3df dir = m_direction;

	// update the the rotation by the servos values
	irr::core::matrix4 transformation = m_object->getAbsoluteTransformation();

	irr::core::vector3df angle_diff = calc_angle_diff(time_delta);
	irr::core::matrix4 rot_mat;
	rot_mat.setRotationDegrees(angle_diff);
	transformation *= rot_mat;

	// update the direction by the rotation values
	irr::core::vector3df rot = transformation.getRotationDegrees();
	dir.rotateYZBy(rot.X);
	dir.rotateXZBy(-1.*rot.Y);
	dir.rotateXYBy(rot.Z);

	// update the sensors
	SensorGenerator::vector_t gyro_data = {{ angle_diff.X, angle_diff.Y, angle_diff.Z }};
	m_gyro.set_data(gyro_data);

	// update the position by the dirction value
	pos  += dir*time_delta*m_params.get_speed();

	// tell the object all the info
	m_object->setRotation(transformation.getRotationDegrees());
	m_object->setPosition(pos);
	m_object->setRotation(rot);
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

