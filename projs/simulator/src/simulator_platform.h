#ifndef SIMULATOR_PLATFORM_H_
#define SIMULATOR_PLATFORM_H_

#include <autopilot/interfaces/plain_platform.h>
#include <boost/shared_ptr.hpp>
#include "plain.h"
#include "wind_gen.h"

namespace simulator {

/**
 * A nice function that creates a complete simulator and returns an autopilot's
 * PlainPlatform struct with all the needed sensors.
 */
boost::shared_ptr<autopilot::NormalPlainPlatform> create_simulator_platform(
		PlainParams plain_params,
		WindGen::Params wind_params,
		lin_algebra::vec3f gps_start_pos = lin_algebra::create_vec3f(0., 0., 0.)
);

} // namespace simulator

#endif /* SIMULATOR_PLATFORM_H_ */
