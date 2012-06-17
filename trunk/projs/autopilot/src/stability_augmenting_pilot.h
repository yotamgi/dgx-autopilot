#ifndef STABILITY_AUGMENTING_PILOT_H_
#define STABILITY_AUGMENTING_PILOT_H_

#include "interfaces/plain_cockpit.h"
#include <stream/stream_utils.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <stdexcept>

namespace autopilot {

/**
 * A pilot for those who want to control the plane's manuevers.
 * This pilot will keep the airplane in a stable angle, while letting the user
 * control the angle value itself.
 */
class StabilityAugmentingPilot {
public:

	StabilityAugmentingPilot(boost::shared_ptr<NormalPlainCockpit> cockpit,
			float pitch_strength,
			float roll_strength);

	/**
	 * Updates one "frame"
	 */
	void update();

	/**
	 * Runs the autopilot system.
	 * It opens a thread and calls the update function in a loop.
	 * This function can also be asked not to open a thread, and thus be blocking.
	 */
	void start(bool open_thread=true);

	/**
	 * Stopps the run function
	 */
	void stop();

	/**
	 * Functions for getting the control on the plane
	 */
	boost::shared_ptr<stream::DataPushStream<float> > get_tilt_control();
	boost::shared_ptr<stream::DataPushStream<float> > get_roll_control();

private:

	static const float UPDATE_RATE = 30.;

	boost::shared_ptr<NormalPlainCockpit> m_cockpit;

	boost::shared_ptr<stream::PushToPopConv<float> > m_tilt_angle;
	boost::shared_ptr<stream::PushToPopConv<float> > m_roll_angle;

	boost::thread m_running_thread;

	float m_pitch_strength;
	float m_roll_strength;

	volatile bool m_running;
};

} // namespace autopilot

#endif /* STABILITY_AUGMENTING_PILOT_H_ */
