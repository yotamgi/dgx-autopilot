#ifndef WAYPOINT_PILOT_H_
#define WAYPOINT_PILOT_H_

#include "interfaces/plain_cockpit.h"
#include <stream/util/lin_algebra.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace autopilot {

struct WaypointPilotParams {

	float max_climbing_angle;
	float climbing_gas;

	float max_decending_angle;
	float decending_gas;

	float avg_gas;
	float avg_pitch;

	float max_tilt_angle;
};

class WaypointPilot {
public:
	WaypointPilot(const WaypointPilotParams& params, boost::shared_ptr<NormalPlainCockpit> cockpit);

	void start(bool open_thread=true);
	void stop();

	struct waypoint {
		waypoint(lin_algebra::vec2f target_, float altitude_);

		lin_algebra::vec2f target;
		float altitude;
	};

	typedef std::vector<waypoint> waypoint_list;

	void set_path(waypoint_list path);
	waypoint_list get_path();

private:

	void fly();

	void maintain_angle(float angle);
	void maintain_pitch(float pitch);
	void maintain_alt(float altitude);
	void maintain_heading(float heading);
	bool nav_to(waypoint waypoint);


	boost::shared_ptr<NormalPlainCockpit> m_cockpit;

	/**
	 * The current plain path
	 */
	waypoint_list m_path;

	/**
	 * members for the new path synching.
	 * Because the std::vector is not thread safe, there is a mechanism to avoid
	 * thread safty problem. when the user tries to set a new path, the path is
	 * inserted to the new path and the m_waiting path is set to true in an
	 * atomic action.
	 */
	waypoint_list m_new_path;
	volatile bool m_waiting_path;

	/** Indicates if the flying mech is working */
	volatile bool m_running;

	/** The flying mech thread */
	boost::thread m_running_thread;

	/** The Pilot params as set in the Ctor */
	WaypointPilotParams m_params;
};


}  // namespace autopilot

#endif /* WAYPOINT_PILOT_H_ */