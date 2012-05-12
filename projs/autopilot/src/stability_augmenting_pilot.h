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

	struct Params {
		float max_pitch_angle;
		float max_tilt_angle;
	};

	StabilityAugmentingPilot(const Params& params, boost::shared_ptr<NormalPlainCockpit> cockpit);

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

	boost::shared_ptr<NormalPlainCockpit> m_cockpit;

	boost::shared_ptr<stream::PushToPopConv<float> > m_tilt_strenth;
	boost::shared_ptr<stream::PushToPopConv<float> > m_roll_strenth;

	boost::thread m_running_thread;

	const Params m_params;

	volatile bool m_running;
};

} // namespace autopilot

#endif /* STABILITY_AUGMENTING_PILOT_H_ */
