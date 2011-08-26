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
		m_direction(irr::core::vector3df(0.0f, 0.0f, 1.0f))
{
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
	pos  += m_direction*time_delta*10.0f;
	m_object->setPosition(pos);
}




}  // namespace simulator
