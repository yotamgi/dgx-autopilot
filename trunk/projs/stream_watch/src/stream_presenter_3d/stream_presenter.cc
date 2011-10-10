#include "stream_presenter.h"
#include <iostream>

#ifndef MEDIA_DIR
#error "MEDIA_DIR macro must be defined and contain the media dir path"
#define MEDIA_DIR ""
#endif


using namespace irr;

StreamPresenter::~StreamPresenter()
{
	m_device->drop();
}

StreamPresenter::StreamPresenter():
		m_running(false)
{
}

void StreamPresenter::setAngleStream(boost::shared_ptr<stream::VecGenerator<float,3> > angle_stream) {
	m_angle = angle_stream;
}

void StreamPresenter::setVecStream(boost::shared_ptr<stream::VecGenerator<float,3> > vec_stream) {
	m_vec = vec_stream;
}


void StreamPresenter::run(bool open_thread) {

	if (open_thread) {
		m_thread.reset(new boost::thread(&StreamPresenter::run, this, false));
	} else {

		irr::video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;

		m_device = createDevice(driverType,
				core::dimension2d<u32>(640, 480), 16, false, false, false);

		if (m_device == 0)
			throw std::runtime_error("could not create device");

		video::IVideoDriver* driver = m_device->getVideoDriver();
		scene::ISceneManager* smgr = m_device->getSceneManager();

		// initalize the meshs
		m_object = smgr->addMeshSceneNode(smgr->getMesh(MEDIA_DIR "/F16_Thuderbirds.x"));
		if (!m_object) {
			throw std::runtime_error("Could not load the plane mesh");
		}

		m_object->setPosition(core::vector3df(0.f, 0.f, 30.f));
		m_object->setScale(irr::core::vector3df(10.0f, 10.0f, 10.0f));
		m_object->setMaterialTexture(0, m_device->getVideoDriver()->getTexture(MEDIA_DIR "/F16_Thuderbirds.bmp"));
		m_object->setMaterialFlag(irr::video::EMF_LIGHTING, false);

		// add terrain scene node
		scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
			MEDIA_DIR "/terrain-heightmap.bmp",
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
				driver->getTexture(MEDIA_DIR "/terrain-texture.jpg"));
		terrain->setMaterialTexture(1,
				driver->getTexture(MEDIA_DIR "/detailmap3.jpg"));

		terrain->setMaterialType(video::EMT_DETAIL_MAP);

		terrain->scaleTexture(1.0f, 20.0f);


		/*
		To be able to look at and move around in this scene, we create a first
		person shooter style camera and make the mouse cursor invisible.
		*/
		smgr->addCameraSceneNodeFPS();
		m_device->getCursorControl()->setVisible(false);


		int lastFPS = -1;
		m_running = true;

		while(m_device->run() && m_running)
		{
			// draw the angle
			if (m_angle) {
				// set the rotation object
				irr::core::matrix4 rotx, roty, rotz;
				stream::VecGenerator<float,3>::vector_t curr_angle = m_angle->get_data();
				rotx.setRotationDegrees(core::vector3df(curr_angle[0], 0., 0.));
				roty.setRotationDegrees(core::vector3df(0., curr_angle[1], 0.));
				rotz.setRotationDegrees(core::vector3df(0., 0., curr_angle[2]));

				irr::core::matrix4 trans = roty * rotx * rotz;
				m_object->setRotation(trans.getRotationDegrees());

				m_object->setVisible(true);
			} else {
				m_object->setVisible(false);
			}

			m_device->getVideoDriver()->beginScene(true, true, video::SColor(255,113,113,133));

			m_device->getSceneManager()->drawAll(); // draw the 3d scene
			m_device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

			// draw the line
			if (m_vec) {
				stream::VecGenerator<float,3>::vector_t curr_vec = m_vec->get_data();
				driver->draw3DLine(core::vector3df(0., 0., 30.),
						core::vector3df(0., 0., 30.) + core::vector3df(curr_vec[0], curr_vec[1], curr_vec[2]));
			}

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
