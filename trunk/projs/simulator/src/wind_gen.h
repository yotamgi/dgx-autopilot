#ifndef WIND_GEN_H_
#define WIND_GEN_H_

#include <stream/util/lin_algebra.h>
#include <stream/util/time.h>

namespace simulator {

class WindGen {
public:

	struct Params {
		lin_algebra::vec3f const_wind;
		float long_disturbance_strength;
		float little_disturbance_strength;
	};

	WindGen(Params params);

	lin_algebra::vec3f get_wind();

private:
	Params m_params;
	lin_algebra::vec3f m_curr_disturbance;
	Timer m_disturbance_timer;
	float m_curr_disturbance_period;
};

}  // namespace simulator


#endif /* WIND_GEN_H_ */
