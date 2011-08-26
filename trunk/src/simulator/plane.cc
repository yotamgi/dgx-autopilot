#include "plane.h"

namespace simulator {


PlanesParams::PlanesParams(const std::string& mesh_file,
				const std::string& texture_file,
				const irr::core::vector3df scale):
	m_mesh_file(mesh_file),
	m_texture_file(texture_file),
	m_scale(scale)
{}

Plane::Plane(irr::IrrlichtDevice* device,
		irr::core::vector3df start_pos,
		const PlanesParams plane_params):
		FlyingObject(device),
		m_direction(irr::core::vector3df(0.0f, 0.0f, -1.0f))
{
	m_servos.yaw_percentage = 50.0f;
	m_servos.pitch_percentage = 50.0f;
	m_servos.ailron_percentage = 50.0f;

	irr::scene::ISceneManager* smgr = m_device->getSceneManager();

	m_object = smgr->addMeshSceneNode(smgr->getMesh(plane_params.get_mesh_file().c_str()));
	if (!m_object) {
		throw std::runtime_error("Could not load the plane mesh");
	}

	m_object->setPosition(start_pos);
	m_object->setScale(plane_params.get_scale());
	m_object->setMaterialTexture(0, m_device->getVideoDriver()->getTexture(plane_params.get_texture_file().c_str()));
	m_object->setMaterialFlag(irr::video::EMF_LIGHTING, false);
}


void Plane::update(float time_delta) {
	irr::core::vector3df pos = m_object->getPosition();
	irr::core::vector3df rot = m_object->getRotation();
	irr::core::vector3df dir = m_direction;

	dir.rotateYZBy(rot.X);
	dir.rotateXZBy(-1.*rot.Y);
	dir.rotateXYBy(rot.Z);

	pos  += dir*time_delta*30.0f;
	rot += irr::core::vector3df(
		((m_servos.pitch_percentage   - 50.0f)/50.0f)*30.0f*time_delta,
		((m_servos.yaw_percentage 	  - 50.0f)/50.0f)*30.0f*time_delta,
		((m_servos.ailron_percentage  - 50.0f)/50.0f)*30.0f*time_delta
	);

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

