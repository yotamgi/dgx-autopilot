#ifndef WAYPOINT_PILOT_H_
#define WAYPOINT_PILOT_H_

#include "interfaces/plain_cockpit.h"
#include <stream/util/lin_algebra.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace autopilot {

struct WaypointPilotParams {

	float avg_climbing_angle;
	float max_climbing_angle;

	float avg_decending_angle;
	float max_decending_angle;

	float max_tilt_angle;
};

class WaypointPilot {
public:
	WaypointPilot(const WaypointPilotParams& params, boost::shared_ptr<NormalPlainCockpit> cockpit);

	void start(bool open_thread=true);
	void stop();

	void to_waypoint(lin_algebra::vec2f target, float altitude = -1.);
	void set_altitude(float altitude = -1.);

private:

	void fly();

	boost::shared_ptr<NormalPlainCockpit> m_cockpit;

	lin_algebra::vec2f m_target;
	float m_altitude;

	bool m_running;
	boost::thread m_running_thread;
	WaypointPilotParams m_params;
};


}  // namespace autopilot

#endif /* WAYPOINT_PILOT_H_ */
