
#ifndef CAMERA_H_
#define CAMERA_H_

#include "flying_object.h"
#include <irrlicht/irrlicht.h>
#include <boost/shared_ptr.hpp>

namespace simulator {

/**
 * Camera with tree typs:
 *  - tracks a flying object from behind
 *  - tracks a flying object from fixed place
 *  - FPS camera
 */
class Camera {
public:
	Camera(irr::IrrlichtDevice* device, boost::shared_ptr<FlyingObject> tracked, irr::core::vector3df pos, float closeness);

	enum CameraType {
		TRACK_BEHIND, TRACK_FIXED, FPS
	};

	void setType(CameraType t) { m_type = t; }

	void update(float time_delta);

private:

	CameraType m_type;

	irr::core::vector3df m_fixed_pos;
	float m_closeness;

	/** needed for calculating the tracked direction */
	irr::core::vector3df m_tracked_prev_pos;

	boost::shared_ptr<FlyingObject> m_tracked;

	irr::scene::ICameraSceneNode* m_tracking_camera_node;
	irr::scene::ICameraSceneNode* m_fps_camera_node;

	irr::IrrlichtDevice* m_device;
};

}  // namespace simulator


#endif /* CAMERA_H_ */
