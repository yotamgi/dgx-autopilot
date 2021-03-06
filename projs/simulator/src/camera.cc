#include "camera.h"

namespace simulator {

Camera::Camera(irr::IrrlichtDevice* device, boost::shared_ptr<FlyingObject> tracked, irr::core::vector3df pos, float closeness):
		m_type(Camera::TRACK_BEHIND),
		m_fixed_pos(pos),
		m_closeness(closeness),
		m_tracked(tracked),
		m_device(device)
{
	m_tracking_camera_node = m_device->getSceneManager()->addCameraSceneNode();
	m_fps_camera_node = m_device->getSceneManager()->addCameraSceneNodeFPS();

	m_tracked_prev_pos = m_tracked->get_pos() - irr::core::vector3df(1.f, 1.f, 1.f);

}

void Camera::update(float time_delta) {
	irr::core::vector3df tracked_pos;
	irr::core::vector3df tracked_dir;

	switch (m_type) {
	case TRACK_BEHIND:

		m_device->getSceneManager()->setActiveCamera(m_tracking_camera_node);

		tracked_pos = m_tracked->get_pos();
		tracked_dir = (tracked_pos - m_tracked_prev_pos).normalize();
		m_tracked_low_pass = m_tracked_low_pass*(1 - 4*time_delta) + tracked_dir*4*time_delta;

		m_tracking_camera_node->setTarget(tracked_pos);
		m_tracking_camera_node->setPosition(tracked_pos - m_tracked_low_pass*m_closeness);

		m_tracked_prev_pos = tracked_pos;

		break;


	case TRACK_FIXED:

		m_device->getSceneManager()->setActiveCamera(m_tracking_camera_node);

		tracked_pos = m_tracked->get_pos();

		m_tracking_camera_node->setPosition(m_fixed_pos);
		m_tracking_camera_node->setTarget(tracked_pos);

		break;

	case FPS:
		m_device->getSceneManager()->setActiveCamera(m_fps_camera_node);
		break;
	}
}

}  // namespace simulator
