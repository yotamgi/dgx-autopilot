#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "plain.h"
#include "wind_gen.h"
#include "camera.h"

#include <irrlicht/irrlicht.h>
#include <cmath>
#include <sys/time.h>
#include <irrlicht/irrlicht.h>
#include <stdexcept>

namespace simulator {

class Simulator {
public:
	Simulator(PlainParams plain_params, WindGen::Params wind_params);
	~Simulator();

	boost::shared_ptr<simulator::Plain> get_plane() { return m_plain; }
	boost::shared_ptr<simulator::WindGen> get_wind() { return m_wind_gen; }

	void run();
private:

	/**
	 * To receive events like mouse and keyboard input, or GUI events like "the OK
	 * button has been clicked", we need an object which is derived from the
	 * irr::IEventReceiver object. There is only one method to override:
	 * irr::IEventReceiver::OnEvent(). This method will be called by the engine once
	 * when an event happens. What we really want to know is whether a key is being
	 * held down, and so we will remember the current state of each key.
	 */
	class EventReceiver : public irr::IEventReceiver
	{
	public:
		EventReceiver();

		// This is the one method that we have to implement
		virtual bool OnEvent(const irr::SEvent& event);

		// This is used to check whether a key is being held down
		virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const;

	private:

		// We use this array to store the current state of each key
		bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
	};

	irr::IrrlichtDevice* m_device;
	irr::scene::ITerrainSceneNode* m_terrain;

    boost::shared_ptr<simulator::Camera> m_camera;
	boost::shared_ptr<simulator::Plain> m_plain;
	boost::shared_ptr<WindGen> m_wind_gen;

	EventReceiver receiver;
};

} // namespace simulator

#endif /* SIMULATOR_H_ */
