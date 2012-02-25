#include "3d_stream_view.h"
#include <boost/foreach.hpp>

#ifndef MEDIA_DIR
#error "MEDIA_DIR macro must be defined and contain the media dir path"
#define MEDIA_DIR ""
#endif


using namespace irr;

namespace gs {

StreamView3d::~StreamView3d()
{
	if (m_device) {
		m_device->closeDevice();
		m_device->drop();
	}
}

StreamView3d::StreamView3d(float update_time, QSize size, QWidget* parent):
		QWidget(parent),
		m_update_time(update_time)
{
	setFixedSize(size);

	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_PaintOnScreen, true);
	setAutoFillBackground(false);
}

void StreamView3d::start() {
	// init irrlicht device
	SIrrlichtCreationParameters params;
    params.AntiAlias = 0;
    params.DriverType = video::EDT_OPENGL;
    params.WindowId = reinterpret_cast<void*>(winId());
    params.WindowSize.Width = width();
    params.WindowSize.Height = height();
    params.Bits = 32;
    params.Stencilbuffer = true;
    params.Doublebuffer = true;
    params.Stereobuffer = false;

	m_device = createDeviceEx(params);
	if (m_device == 0) {
		throw std::runtime_error("could not create device");
	}

	video::IVideoDriver* driver = m_device->getVideoDriver();
	scene::ISceneManager* smgr = m_device->getSceneManager();

	// add terrain scene node
	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
		MEDIA_DIR "/3d/terrain-heightmap.bmp",
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
			driver->getTexture(MEDIA_DIR "/3d/terrain-texture.jpg"));
	terrain->setMaterialTexture(1,
			driver->getTexture(MEDIA_DIR "/3d/detailmap3.jpg"));
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

	// create the static camera
	irr::core::vector3df pos    = irr::core::vector3df(0., 0., 30.);
	irr::core::vector3df lookat = irr::core::vector3df(0., 0., 0.);
	smgr->addCameraSceneNode(0, pos, lookat);
	m_device->getCursorControl()->setVisible(true);

	// Set the light
	smgr->setAmbientLight(video::SColorf(0.3,0.3,0.3,1));
	smgr->addLightSceneNode(0, core::vector3df(-5,-5,-5), video::SColorf(1.f, 1., 1.f), 100.0f, 1);

	// start the update timer
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer->start(1000*m_update_time);
}

void StreamView3d::paintEvent(QPaintEvent* event) {
	update();
}

void StreamView3d::addAngleStream(stream3ptr angle_stream, irr::core::vector3df pos) {
	m_angles.push_back(boost::shared_ptr<AnglePresenter>(
			new AnglePresenter(angle_stream, pos)
	));
}

void StreamView3d::addVecStream(stream3ptr vec_stream, irr::core::vector3df pos) {
	m_vecs.push_back(boost::shared_ptr<VecPresenter>(
			new VecPresenter(vec_stream, pos)
	));
}

void StreamView3d::addSizeStream(streamfptr size_stream,
		irr::core::vector2df scale,
		irr::core::vector2df pos)
{
	m_sizes.push_back(boost::shared_ptr<SizePresenter>(
			new SizePresenter(size_stream, pos, scale)
	));
}

void StreamView3d::update() {
	if (m_device) {

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
	}
}

StreamView3d::AnglePresenter::AnglePresenter(StreamView3d::stream3ptr angle_stream, irr::core::vector3df pos):
	m_angle_stream(angle_stream),
	m_pos(pos),
	m_rot(0., 180., 0.)
{}

void StreamView3d::AnglePresenter::initalize(irr::IrrlichtDevice* m_device) {
	scene::ISceneManager* smgr = m_device->getSceneManager();

	// initalize the meshs
	m_object = smgr->addMeshSceneNode(smgr->getMesh(MEDIA_DIR "/3d/F16_Thuderbirds.x"));
	if (!m_object) {
		throw std::runtime_error("Could not load the plane mesh");
	}

	m_object->setPosition(m_pos);
	m_object->setScale(irr::core::vector3df(10.0f, 10.0f, 10.0f));
	m_object->setMaterialTexture(0, m_device->getVideoDriver()->getTexture(MEDIA_DIR "/3d/F16_Thuderbirds.bmp"));
	m_object->setMaterialFlag(irr::video::EMF_LIGHTING, false);

	m_object->setVisible(true);
}

void StreamView3d::AnglePresenter::draw(irr::IrrlichtDevice* m_device) {
	irr::core::matrix4 rotx, roty, rotz;
	lin_algebra::vec3f curr_angle = m_angle_stream->get_data();
	rotx.setRotationDegrees(core::vector3df(curr_angle[0], 0., 0.));
	roty.setRotationDegrees(core::vector3df(0., curr_angle[1], 0.));
	rotz.setRotationDegrees(core::vector3df(0., 0., curr_angle[2]));

	irr::core::matrix4 obj_rot;
	obj_rot.setRotationDegrees(m_rot);
	irr::core::matrix4 trans = obj_rot * roty * rotx * rotz;
	m_object->setRotation(trans.getRotationDegrees());
}

StreamView3d::VecPresenter::VecPresenter(StreamView3d::stream3ptr Vec_stream, irr::core::vector3df pos):
	m_vec_stream(Vec_stream),
	m_pos(pos),
	m_scale(10.0f, 10.0f, 10.0f)
{}

void StreamView3d::VecPresenter::initalize(irr::IrrlichtDevice* m_device) {
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

void StreamView3d::VecPresenter::draw(irr::IrrlichtDevice* m_device) {
	lin_algebra::vec3f vec_raw = m_vec_stream->get_data();
	irr::core::vector3df vec(vec_raw[2], vec_raw[1], vec_raw[0]);

	float vec_len = vec.getLength();
	vec.normalize();

	m_object->setRotation(vec.getSphericalCoordinateAngles());
	m_object->setScale(irr::core::vector3df(m_scale.X/2., m_scale.Y*vec_len, m_scale.Z/2.));
}

StreamView3d::SizePresenter::SizePresenter(StreamView3d::streamfptr size_stream,
		irr::core::vector2df pos,
		irr::core::vector2df scale):
	m_size_stream(size_stream),
	m_pos(pos),
	m_scale(scale.X * 25., scale.Y * 100.)
{}

void StreamView3d::SizePresenter::initalize(irr::IrrlichtDevice* m_device) {
}

void StreamView3d::SizePresenter::draw(irr::IrrlichtDevice* m_device) {
	float size = m_size_stream->get_data();

	m_device->getVideoDriver()->draw2DRectangle(video::SColor( 255, 0, 100, 150 ),
			irr::core::rect<int32_t>(m_pos.X, 470 - (m_pos.Y + size*m_scale.Y),
					(m_pos.X + m_scale.X), 470 - m_pos.Y)
	);
}

} // namespace gs
