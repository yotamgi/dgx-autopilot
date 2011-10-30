#include "stream_presenter.h"
#include <iostream>
#include <boost/foreach.hpp>

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

void StreamPresenter::addAngleStream(stream3ptr angle_stream, irr::core::vector3df pos) {
	m_angles.push_back(boost::shared_ptr<AnglePresenter>(
			new AnglePresenter(angle_stream, pos)
	));
}

void StreamPresenter::addVecStream(stream3ptr vec_stream, irr::core::vector3df pos) {
	m_vecs.push_back(boost::shared_ptr<VecPresenter>(
			new VecPresenter(vec_stream, pos)
	));
}

void StreamPresenter::addSizeStream(streamfptr size_stream,
		irr::core::vector2df scale,
		irr::core::vector2df pos)
{
	m_sizes.push_back(boost::shared_ptr<SizePresenter>(
			new SizePresenter(size_stream, pos, scale)
	));
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

		// Initalize the presenters
		BOOST_FOREACH(boost::shared_ptr<AnglePresenter> angle, m_angles) {
			angle->initalize(m_device);
		}
		BOOST_FOREACH(boost::shared_ptr<VecPresenter> vec, m_vecs) {
			vec->initalize(m_device);
		}
		BOOST_FOREACH(boost::shared_ptr<SizePresenter> size, m_sizes) {
			size->initalize(m_device);
		}

		/*
		To be able to look at and move around in this scene, we create a first
		person shooter style camera and make the mouse cursor invisible.
		*/
		irr::scene::ISceneNode* camera = smgr->addCameraSceneNodeFPS(0, 70, 0.05);
		camera->setPosition(irr::core::vector3df(0., 0., -30.));
		m_device->getCursorControl()->setVisible(false);

		// Set the light
		smgr->setAmbientLight(video::SColorf(0.3,0.3,0.3,1));
		smgr->addLightSceneNode( 0, core::vector3df(-5,-5,-5), video::SColorf(1.f, 1., 1.f), 100.0f, 1 );

		int lastFPS = -1;
		m_running = true;

		while(m_device->run() && m_running)
		{

			m_device->getVideoDriver()->beginScene(true, true, video::SColor(255,113,113,133));

			m_device->getSceneManager()->drawAll(); // draw the 3d scene
			m_device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

			BOOST_FOREACH(boost::shared_ptr<VecPresenter> vec, m_vecs) {
				vec->draw(m_device);
			}
			BOOST_FOREACH(boost::shared_ptr<AnglePresenter> angle, m_angles) {
				angle->draw(m_device);
			}
			BOOST_FOREACH(boost::shared_ptr<SizePresenter> size, m_sizes) {
				size->draw(m_device);
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

StreamPresenter::AnglePresenter::AnglePresenter(StreamPresenter::stream3ptr angle_stream, irr::core::vector3df pos):
	m_angle_stream(angle_stream),
	m_pos(pos)
{}

void StreamPresenter::AnglePresenter::initalize(irr::IrrlichtDevice* m_device) {
	scene::ISceneManager* smgr = m_device->getSceneManager();

	// initalize the meshs
	m_object = smgr->addMeshSceneNode(smgr->getMesh(MEDIA_DIR "/F16_Thuderbirds.x"));
	if (!m_object) {
		throw std::runtime_error("Could not load the plane mesh");
	}

	m_object->setPosition(m_pos);
	m_object->setScale(irr::core::vector3df(10.0f, 10.0f, 10.0f));
	m_object->setMaterialTexture(0, m_device->getVideoDriver()->getTexture(MEDIA_DIR "/F16_Thuderbirds.bmp"));
	m_object->setMaterialFlag(irr::video::EMF_LIGHTING, false);

	m_object->setVisible(true);
}

void StreamPresenter::AnglePresenter::draw(irr::IrrlichtDevice* m_device) {
	irr::core::matrix4 rotx, roty, rotz;
	stream3::vector_t curr_angle = m_angle_stream->get_data();
	rotx.setRotationDegrees(core::vector3df(curr_angle[0], 0., 0.));
	roty.setRotationDegrees(core::vector3df(0., curr_angle[1], 0.));
	rotz.setRotationDegrees(core::vector3df(0., 0., curr_angle[2]));

	irr::core::matrix4 trans = roty * rotx * rotz;
	m_object->setRotation(trans.getRotationDegrees());
}

StreamPresenter::VecPresenter::VecPresenter(StreamPresenter::stream3ptr Vec_stream, irr::core::vector3df pos):
	m_vec_stream(Vec_stream),
	m_pos(pos),
	m_scale(10.0f, 10.0f, 10.0f)
{}

void StreamPresenter::VecPresenter::initalize(irr::IrrlichtDevice* m_device) {
	scene::ISceneManager* smgr = m_device->getSceneManager();

	// initalize the meshs
	m_object = smgr->addMeshSceneNode(smgr->addArrowMesh("arrow"));
	if (!m_object) {
		throw std::runtime_error("Could not load the plane mesh");
	}

	m_object->setPosition(m_pos);
	m_object->setMaterialFlag(irr::video::EMF_LIGHTING, true);
	m_object->setVisible(true);
}

void StreamPresenter::VecPresenter::draw(irr::IrrlichtDevice* m_device) {
	stream3::vector_t vec_raw = m_vec_stream->get_data();
	irr::core::vector3df vec(vec_raw[2], vec_raw[1], vec_raw[0]);

	float vec_len = vec.getLength();
	vec.normalize();

	m_object->setRotation(vec.getSphericalCoordinateAngles());
	m_object->setScale(irr::core::vector3df(m_scale.X/2., m_scale.Y*vec_len, m_scale.Z/2.));
//	m_device->getVideoDriver()->setTransform(video::ETS_WORLD, core::IdentityMatrix);
//	m_device->getVideoDriver()->draw3DLine(m_pos, m_pos + vec*10.);
}

StreamPresenter::SizePresenter::SizePresenter(StreamPresenter::streamfptr size_stream,
		irr::core::vector2df pos,
		irr::core::vector2df scale):
	m_size_stream(size_stream),
	m_pos(pos),
	m_scale(scale.X * 25., scale.Y * 100.)
{}

void StreamPresenter::SizePresenter::initalize(irr::IrrlichtDevice* m_device) {
}

void StreamPresenter::SizePresenter::draw(irr::IrrlichtDevice* m_device) {
	float size = m_size_stream->get_data();

	m_device->getVideoDriver()->draw2DRectangle(video::SColor( 255, 0, 100, 150 ),
			irr::core::rect<int32_t>(m_pos.X, 470 - (m_pos.Y + size*m_scale.Y),
					(m_pos.X + m_scale.X), 470 - m_pos.Y)
	);
}
