/** Example 004 Movement

This Tutorial shows how to move and animate SceneNodes. The
basic concept of SceneNodeAnimators is shown as well as manual
movement of nodes using the keyboard.  We'll demonstrate framerate
independent movement, which means moving by an amount dependent
on the duration of the last run of the Irrlicht loop.

Example 19.MouseAndJoystick shows how to handle those kinds of input.

As always, I include the header files, use the irr namespace,
and tell the linker to link with the .lib file.
*/
#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

#include "plane.h"
#include "camera.h"
#include "sensors/simulator_gyro.h"
#include <irrlicht/irrlicht.h>
#include <cmath>
#include <boost/make_shared.hpp>

#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <stream/filters/integral_filter.h>
#include <stream/filters/gyro_to_av_matrix.h>
#include <stream/filters/acc_compass_rot.h>
#include <stream/filters/matrix_to_euler_filter.h>

#include <boost/thread.hpp>

using namespace irr;

typedef stream::DataPopStream<lin_algebra::vec3f> vec3stream;
typedef boost::shared_ptr<vec3stream> vec3stream_ptr;


/*
To receive events like mouse and keyboard input, or GUI events like "the OK
button has been clicked", we need an object which is derived from the
irr::IEventReceiver object. There is only one method to override:
irr::IEventReceiver::OnEvent(). This method will be called by the engine once
when an event happens. What we really want to know is whether a key is being
held down, and so we will remember the current state of each key.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

void export_import(simulator::Plane& p, vec3stream_ptr gyro) {

	while (true) {
		while (!p.data_ready());
		try {
			boost::shared_ptr<stream::TcpipClient> client =
					boost::make_shared<stream::TcpipClient>("localhost", 0x6060);
			stream::StreamConnection conn(client);
			conn.export_pop_stream<lin_algebra::vec3f>(gyro, "simulator_gyro");
			conn.export_pop_stream<lin_algebra::vec3f>(p.acc_gen(), "simulator_acc");
			conn.export_pop_stream<lin_algebra::vec3f>(p.compass_gen(), "simulator_compass");

			conn.export_push_stream<float>(p.get_pitch_servo(), "simulator_pitch_servo");
			conn.export_push_stream<float>(p.get_tilt_servo(), "simulator_tilt_servo");
			conn.export_push_stream<float>(p.get_gas_servo(), "simulator_gas_servo");
			conn.run(true);

			p.set_gps_pos_listener(conn.import_push_stream<lin_algebra::vec3f>("gps_pos_reciever"));
			p.set_gps_speed_dir_listener(conn.import_push_stream<float>("gps_speed_dir_reciever"));
			p.set_gps_speed_mag_listener(conn.import_push_stream<float>("gps_speed_mag_reciever"));

			// now wait
			while (true) usleep(10000000);
		} catch (stream::ConnectionExceptioin& e) {
			std::cout << "Connection died" << std::endl;
		}
	}
}

/*
The event receiver for keeping the pressed keys is ready, the actual responses
will be made inside the render loop, right before drawing the scene. So lets
just create an irr::IrrlichtDevice and the scene node we want to move. We also
create some other additional scene nodes, to show that there are also some
different possibilities to move and animate scene nodes.
*/
int main()
{
	// create device
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
	MyEventReceiver receiver;

	IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<u32>(640, 480), 16, false, false, false, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();


	simulator::PlainParams plane_params(
			 "media/pf-cessna-182.x",
			 "media/pf-cessna-182.bmp",
			 irr::core::vector3df(1., 1., 1.),
			 irr::core::vector3df(0., 180., 0.),
			 200.0f,
			 100.0f,
			 100.0f,
			 6.5f,  // mass
			 40.0f,  // engine power
			 0.15f, // drag
			 1.0f, // wing area
			 -3.); // lift

//	simulator::PlainParams plane_params(
//			 "media/pf-cessna-182.x",
//			 "media/pf-cessna-182.bmp",
//			 irr::core::vector3df(4.0f, 4.0f, 4.0f),
//			 200.0f,
//			 60.0f,
//			 100.0f,
//			 1000.0f,  // mass
//			 3000.0f,  // engine power
//			 0.8f, // drag
//			 13.0f, // wing area
//			 -5.); // lift


	simulator::Plane p(device, core::vector3df(0.0f, 0.0f, 0.0f), plane_params);
	boost::shared_ptr<simulator::SimulatorGyroSensor> gyro_sensor(
			new simulator::SimulatorGyroSensor(plane_params.get_rot())
	);
	p.add_sensor(gyro_sensor);

	// inital servo data
	p.get_pitch_servo()->set_data(50.);
	p.get_tilt_servo()->set_data(50.);
	p.get_yaw_servo()->set_data(50.);

	// export all the sensors
	boost::thread t(export_import, boost::ref(p), gyro_sensor);

    // add terrain scene node
    scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
        "media/terrain-heightmap.bmp",
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

    terrain->setMaterialFlag(video::EMF_LIGHTING, false);

    terrain->setMaterialTexture(0,
            driver->getTexture("media/terrain-texture.jpg"));
    terrain->setMaterialTexture(1,
            driver->getTexture("media/detailmap3.jpg"));

    terrain->setMaterialType(video::EMT_DETAIL_MAP);

    terrain->scaleTexture(1.0f, 20.0f);


    simulator::Camera c(device, &p, irr::core::vector3df(4.0, 4.0, -4.0), 2.0);
    c.setType(simulator::Camera::TRACK_FIXED);

	/*
	We have done everything, so lets draw it. We also write the current
	frames per second and the name of the driver to the caption of the
	window.
	*/
	int lastFPS = -1;

	// In order to do framerate independent movement, we have to know
	// how long it was since the last frame
	u32 then = device->getTimer()->getTime();
	f32 frameDeltaTime = 0.;

	while(device->run())
	{
		// Work out a frame delta time.
		const u32 now = device->getTimer()->getTime();
		frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;

		p.update(frameDeltaTime);
		c.update(frameDeltaTime);

		driver->beginScene(true, true, video::SColor(255,113,113,133));

		smgr->drawAll(); // draw the 3d scene
		device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

		driver->endScene();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			core::stringw tmp(L"DGX Project Plane Simulator");
			tmp += driver->getName();
			tmp += L"] fps: ";
			tmp += fps;

			device->setWindowCaption(tmp.c_str());
			lastFPS = fps;
		}

		///////////////////////////////////////
		// Update the plane according to the keys
		//////////////
		if (receiver.IsKeyDown(KEY_UP)) p.force_pitch(90.);
		else if (receiver.IsKeyDown(KEY_DOWN)) p.force_pitch(10.);
		else p.unforce_pitch();

		if (receiver.IsKeyDown(KEY_LEFT))  p.force_tilt(90.);
		else if (receiver.IsKeyDown(KEY_RIGHT)) p.force_tilt(10.);
		else p.unforce_tilt();

		if (receiver.IsKeyDown(KEY_KEY_V)) c.setType(simulator::Camera::FPS);
		if (receiver.IsKeyDown(KEY_KEY_X)) c.setType(simulator::Camera::TRACK_BEHIND);
		if (receiver.IsKeyDown(KEY_KEY_C)) c.setType(simulator::Camera::TRACK_FIXED);
		if (receiver.IsKeyDown(KEY_KEY_A)) p.get_gas_servo()->set_data(100.);
		if (receiver.IsKeyDown(KEY_KEY_Z)) p.get_gas_servo()->set_data(0.);
	}

	/*
	In the end, delete the Irrlicht device.
	*/
	device->drop();
	
	t.detach();

	return 0;
}

/*
That's it. Compile and play around with the program.
**/
