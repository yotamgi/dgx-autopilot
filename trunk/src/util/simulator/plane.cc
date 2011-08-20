#include "plane.h"

namespace simulator {


Plane::Plane(irr::IrrlichtDevice* device,
		irr::core::vector3df start_pos):
		FlyingObject(device),
		m_direction(irr::core::vector3df(0.0f, 0.0f, 1.0f))
{
	irr::scene::ISceneManager* smgr = m_device->getSceneManager();

	m_object = smgr->addMeshSceneNode(smgr->getMesh("F16_Thuderbirds.x"));
	if (!m_object) {
		throw std::runtime_error("Could not load the plane mesh");
	}

	m_object->setPosition(start_pos);
	m_object->setScale(irr::core::vector3df(10.0f, 10.0f, 10.0f));
	m_object->setMaterialTexture(0, m_device->getVideoDriver()->getTexture("F16_Thuderbirds.bmp"));
	m_object->setMaterialFlag(irr::video::EMF_LIGHTING, false);
}


void Plane::update(float time_delta) {
	irr::core::vector3df pos = m_object->getPosition();
	pos  += m_direction*time_delta;
	m_object->setPosition(pos);
}




}  // namespace simulator
