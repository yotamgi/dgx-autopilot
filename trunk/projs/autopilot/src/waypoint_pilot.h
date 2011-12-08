#ifndef WAYPOINT_PILOT_H_
#define WAYPOINT_PILOT_H_

#include "cockpit.h"
#include <stream/util/lin_algebra.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace autopilot {

class WaypointPilot {
public:
	WaypointPilot(boost::shared_ptr<Cockpit> cockpit);

	void start(bool open_thread=true);
	void stop();

	void to_waypoint(lin_algebra::vec2f target, float altitude = -1.);
	void set_altitude(float altitude = -1.);

private:

	void fly();

	boost::shared_ptr<Cockpit> m_cockpit;

	lin_algebra::vec2f m_target;
	float m_altitude;

	bool m_running;
	boost::thread m_running_thread;
};


}  // namespace autopilot

#endif /* WAYPOINT_PILOT_H_ */
