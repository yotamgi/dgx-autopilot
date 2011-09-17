#include "direction_presenter.h"
#include <iostream>

using namespace irr;

DirectionPresenter::~DirectionPresenter() {
	m_device->drop();
}

DirectionPresenter::DirectionPresenter():m_running(true) {
	m_curr_angle.ax = 0.f;
	m_curr_angle.ay = 0.f;
	m_curr_angle.az = 0.f;

}

void DirectionPresenter::run(bool open_thread) {

	if (open_thread) {
		m_thread.reset(new boost::thread(&DirectionPresenter::run, this, false));
	} else {

		irr::video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;

		m_device = createDevice(driverType,
				core::dimension2d<u32>(640, 480), 16, false, false, false);

		if (m_device == 0)
			throw std::runtime_error("could not create device");

		video::IVideoDriver* driver = m_device->getVideoDriver();
		scene::ISceneManager* smgr = m_device->getSceneManager();

		m_object = smgr->addMeshSceneNode(smgr->getMesh("angle_presenter_3d/media/F16_Thuderbirds.x"));
		if (!m_object) {
			throw std::runtime_error("Could not load the plane mesh");
		}

		m_object->setPosition(core::vector3df(0.f, 0.f, 30.f));
		m_object->setScale(irr::core::vector3df(10.0f, 10.0f, 10.0f));
		m_object->setMaterialTexture(0, m_device->getVideoDriver()->getTexture("angle_presenter_3d/media/F16_Thuderbirds.bmp"));
		m_object->setMaterialFlag(irr::video::EMF_LIGHTING, false);

		// add terrain scene node
		scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
			"angle_presenter_3d/media/terrain-heightmap.bmp",
			0,                  // parent node
			-1,                 // node id
			core::vector3df(-5000.0f, -400.f, -5000.f),     // position
			core::vector3df(0.f, 0.f, 0.f),     // rotation
			core::vector3df(40.f, 4.4f, 40.f),  // scale
			video::SColor ( 255, 255, 255, 255 ),   // vertexColor
			5,                  // maxLOD
			scene::ETPS_17,             // patchSize
			4                   // smoothFactor
			);

		terrain->setMaterialFlag(video::EMF_LIGHTING, false);

		terrain->setMaterialTexture(0,
				driver->getTexture("angle_presenter_3d/media/terrain-texture.jpg"));
		terrain->setMaterialTexture(1,
				driver->getTexture("angle_presenter_3d/media/detailmap3.jpg"));

		terrain->setMaterialType(video::EMT_DETAIL_MAP);

		terrain->scaleTexture(1.0f, 20.0f);


		/*
		To be able to look at and move around in this scene, we create a first
		person shooter style camera and make the mouse cursor invisible.
		*/
		smgr->addCameraSceneNodeFPS();
		m_device->getCursorControl()->setVisible(false);


		int lastFPS = -1;
		u32 then = m_device->getTimer()->getTime();

		m_running = true;

		while(m_device->run() && m_running)
		{
			irr::core::matrix4 rotx, roty, rotz;
			rotx.setRotationDegrees(core::vector3df(m_curr_angle.ax, 0., 0.));
			roty.setRotationDegrees(core::vector3df(0., m_curr_angle.ay, 0.));
			rotz.setRotationDegrees(core::vector3df(0., 0., m_curr_angle.az));

			irr::core::matrix4 trans = rotx * roty * rotz;
			m_object->setRotation(trans.getRotationDegrees());

			//m_object->setRotation(core::vector3df(m_curr_angle.ax, m_curr_angle.ay, m_curr_angle.az));

			m_device->getVideoDriver()->beginScene(true, true, video::SColor(255,113,113,133));

			m_device->getSceneManager()->drawAll(); // draw the 3d scene
			m_device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

			m_device->getVideoDriver()->endScene();

			int fps = m_device->getVideoDriver()->getFPS();

			if (lastFPS != fps)
			{
				core::stringw tmp(L"Movement Example - Irrlicht Engine [");
				tmp += m_device->getVideoDriver()->getName();
				tmp += L"] fps: ";
				tmp += fps;

				m_device->setWindowCaption(tmp.c_str());
				lastFPS = fps;
			}
		}
		m_device->closeDevice();
		m_device->drop();
	}
}
