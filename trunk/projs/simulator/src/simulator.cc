#include "simulator.h"
#include <boost/make_shared.hpp>

#ifndef MEDIA_DIR
#error "You must define the MEDIA_DIR macro for this to work"
#endif

namespace simulator {

using namespace irr;

Simulator::Simulator(PlainParams plain_params, WindGen::Params wind_params) {

	// create device
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;

	m_device = createDevice(driverType,
			core::dimension2d<u32>(800, 600), 16, false, false, false, &receiver);

	if (m_device == 0) {
		throw std::runtime_error("Could not create Irrlicht graphic device");
	}

	video::IVideoDriver* driver = m_device->getVideoDriver();
	scene::ISceneManager* smgr = m_device->getSceneManager();

	// create the wind_gen
	m_wind_gen = boost::make_shared<WindGen>(wind_params);

	// create the plain
	m_plain = boost::make_shared<simulator::Plain>(m_device, core::vector3df(0.0f, 0.0f, 0.0f), m_wind_gen, plain_params);

	// create the camera
	m_camera = boost::make_shared<Camera>(m_device, m_plain, irr::core::vector3df(4.0, 4.0, -4.0), 2.0);
	m_camera->setType(simulator::Camera::TRACK_FIXED);

    // add terrain scene node
    m_terrain = smgr->addTerrainSceneNode(
        MEDIA_DIR "/terrain-heightmap.bmp",
        0,                  // parent node
        -1,                 // node id
        core::vector3df(-5000.0f, -30., -5000.f),     // position
        core::vector3df(0.f, 0.f, 0.f),     // rotation
        core::vector3df(40.f, 0.2f, 40.f),  // scale
        video::SColor ( 255, 255, 255, 255 ),   // vertexColor
        5,                  // maxLOD
        scene::ETPS_17,             // patchSize
        4                   // smoothFactor
        );

    m_terrain->setMaterialFlag(video::EMF_LIGHTING, false);

    m_terrain->setMaterialTexture(0,
            driver->getTexture(MEDIA_DIR "/terrain-texture.jpg"));
    m_terrain->setMaterialTexture(1,
            driver->getTexture(MEDIA_DIR "/detailmap3.jpg"));

    m_terrain->setMaterialType(video::EMT_DETAIL_MAP);

    m_terrain->scaleTexture(1.0f, 20.0f);
}

Simulator::~Simulator() {
	m_device->drop();
}

void Simulator::run() {

	video::IVideoDriver* driver = m_device->getVideoDriver();
	scene::ISceneManager* smgr = m_device->getSceneManager();

	int lastFPS = -1;

	// In order to do framerate independent movement, we have to know
	// how long it was since the last frame
	struct timeval tv;
	gettimeofday(&tv, NULL);
	double then = tv.tv_sec*1000. + tv.tv_usec/1000.;
	double frameDeltaTime = 0.;

	while(m_device->run())
	{
		// calculate the delta time, and make sure it does not exceed 100 fps
		double now;
		do {
			gettimeofday(&tv, NULL);
			now = tv.tv_sec*1000. +  tv.tv_usec/1000.;
			frameDeltaTime = (now - then) / 1000.;
			usleep(1000);
		} while (frameDeltaTime < 0.01);
		then = now;

		m_plain->update(frameDeltaTime);
		m_camera->update(frameDeltaTime);

		driver->beginScene(true, true, video::SColor(255,113,113,133));

		smgr->drawAll(); // draw the 3d scene
		m_device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

		driver->endScene();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			core::stringw tmp(L"DGX Project Plane Simulator");
			tmp += driver->getName();
			tmp += L"] fps: ";
			tmp += fps;

			m_device->setWindowCaption(tmp.c_str());
			lastFPS = fps;
		}

		///////////////////////////////////////
		// Update the plane according to the keys
		//////////////
		if (receiver.IsKeyDown(KEY_UP)) m_plain->get_elevator_servo()->override(90.);
		else if (receiver.IsKeyDown(KEY_DOWN)) m_plain->get_elevator_servo()->override(10.);
		else m_plain->get_elevator_servo()->stop_override();

		if (receiver.IsKeyDown(KEY_LEFT))  m_plain->get_ailron_servo()->override(90.);
		else if (receiver.IsKeyDown(KEY_RIGHT)) m_plain->get_ailron_servo()->override(10.);
		else m_plain->get_ailron_servo()->stop_override();

		if (receiver.IsKeyDown(KEY_KEY_N))  m_plain->get_rudder_servo()->override(90.);
		else if (receiver.IsKeyDown(KEY_KEY_M)) m_plain->get_rudder_servo()->override(10.);
		else m_plain->get_rudder_servo()->stop_override();

		if (receiver.IsKeyDown(KEY_KEY_V)) m_camera->setType(simulator::Camera::FPS);
		if (receiver.IsKeyDown(KEY_KEY_X)) m_camera->setType(simulator::Camera::TRACK_BEHIND);
		if (receiver.IsKeyDown(KEY_KEY_C)) m_camera->setType(simulator::Camera::TRACK_FIXED);
		if (receiver.IsKeyDown(KEY_KEY_A)) m_plain->get_throttle_servo()->set_data(100.);
		if (receiver.IsKeyDown(KEY_KEY_Z)) m_plain->get_throttle_servo()->set_data(0.);
	}

}

bool Simulator::EventReceiver::OnEvent(const irr::SEvent& event) {
	// Remember whether each key is down or up
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
		KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

	return false;
}

bool Simulator::EventReceiver::IsKeyDown(irr::EKEY_CODE keyCode) const {
	return KeyIsDown[keyCode];
}

Simulator::EventReceiver::EventReceiver()
{
	for (u32 i=0; i<irr::KEY_KEY_CODES_COUNT; ++i)
		KeyIsDown[i] = false;
}


} // namespace simulator
