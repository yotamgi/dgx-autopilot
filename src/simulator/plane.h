/*
 * plane.h
 *
 *  Created on: Aug 20, 2011
 *      Author: yotam
 */

#ifndef PLANE_H_
#define PLANE_H_

#include "flying_object.h"
#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace simulator {

/**
 * A class representing all the parameters that differ from one plane object
 * to another.
 */
struct PlanesParams {

	PlanesParams(const std::string& mesh_file,
				const std::string& texture_file,
				const irr::core::vector3df scale);

	const std::string& get_mesh_file() const { return m_mesh_file; }
	const std::string& get_texture_file() const { return m_texture_file; }
	const irr::core::vector3df& get_scale() const {	return m_scale; }

private:

	std::string m_mesh_file;
	std::string m_texture_file;
	irr::core::vector3df m_scale;
};


/**
 * The plane class - gets a plane param and creates a plane in the field.
 */
class Plane : public FlyingObject {
public:
	Plane(irr::IrrlichtDevice* device,
			irr::core::vector3df start_pos,
			const PlanesParams plane_params);

	virtual void update(float time_delta);

	virtual irr::core::vector3df get_pos() const { return m_object->getPosition(); }

private:

	irr::core::vector3df m_direction;

	irr::scene::ISceneNode * m_object;

};

}  // namespace simulator


#endif /* PLANE_H_ */
