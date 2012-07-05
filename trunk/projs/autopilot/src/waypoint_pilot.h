#ifndef WAYPOINT_PILOT_H_
#define WAYPOINT_PILOT_H_

#include "interfaces/plain_cockpit.h"
#include "stability_augmenting_pilot.h"
#include <stream/util/lin_algebra.h>
#include <stream/util/time.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace autopilot {


class WaypointPilot {
public:

	struct Params {

		float max_climbing_angle;
		float climbing_gas;

		float max_decending_angle;
		float decending_gas;

		float max_roll_angle;

		float pitch_severity;
		float roll_severity;

		float heading_fine_tunning;

		float avg_gas;
		float avg_pitch_angle;

		bool use_airspeed;
		float avg_airspeed;
		float airspeed_severity;
	};

	WaypointPilot(const Params& params, boost::shared_ptr<NormalPlainCockpit> cockpit);

	void start(bool open_thread=true);
	void stop();

	struct waypoint {
		waypoint(lin_algebra::vec2f target_, float altitude_);
		waypoint() {}

		lin_algebra::vec2f target;
		float altitude;
	};

	typedef std::vector<waypoint> waypoint_list;

	Params& params() { return m_params; }

	void set_path(waypoint_list path);
	waypoint_list get_path() { return m_path; }

private:

	void fly();

	void maintain_alt(float altitude);
	void maintain_heading(float heading);
	void maintain_airspeed(float airspeed);
	bool nav_to(waypoint waypoint);


	boost::shared_ptr<NormalPlainCockpit> m_cockpit;

	StabilityAugmentingPilot m_sas_pilot;

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

	waypoint m_roam_waypoint;

	Timer m_t;
	float m_airspeed_low_pass;

	/** The Pilot params as set in the Ctor */
	Params m_params;
};


}  // namespace autopilot

#endif /* WAYPOINT_PILOT_H_ */
