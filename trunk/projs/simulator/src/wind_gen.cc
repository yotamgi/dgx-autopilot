#include "wind_gen.h"

namespace simulator {

static float frand() {
	return float(std::rand())/float(RAND_MAX);
}


WindGen::WindGen(WindGen::Params params):
		m_params(params)
{}

lin_algebra::vec3f WindGen::get_wind() {
	if (m_disturbance_timer.passed() > m_curr_disturbance_period) {
		m_curr_disturbance.randn();
		m_curr_disturbance *= m_params.long_disturbance_strength;
		m_curr_disturbance_period = frand()/2.;
		m_disturbance_timer.reset();
	}

	lin_algebra::vec3f rand;
	rand.randn();
	return m_params.const_wind +
					m_curr_disturbance +
					rand*m_params.little_disturbance_strength;
}

}  // namespace simulator


