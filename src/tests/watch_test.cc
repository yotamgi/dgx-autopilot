#include "util/watch.h"
#include "common/types.h"
#include <iostream>

class SimpleGen : public DataGenerator<vector_t> {
public:

	SimpleGen():m_a(0.0f) {}
	virtual ~SimpleGen(){}

	/**
	 * Return the angular velocity
	 */
	boost::shared_ptr<vector_t> get_data() {
		boost::shared_ptr<vector_t> ans(new vector_t);
		ans->x = m_a;
		ans->y = m_a+1.0f;
		ans->z = m_a+2.0f;
		m_a+=0.01f;
		return ans;
	}

	bool is_data_losed() { return false; }

private:
	float m_a;
};

int main(int argc, char** argv) {

	SimpleGen gen;
	VecWatch watch(&gen, "localhost");

	watch.run();

	while (true) {
		watch.get_data();
		usleep(10000);
	}
	return 0;
}
