#include <iostream>

#include "util/watch.h"

typedef Watch<float,3> VecWatch;


class SimpleGen : public VecGenerator<float,3> {
public:

	SimpleGen():m_a(0.0f) {}
	virtual ~SimpleGen(){}

	/**
	 * Return the angular velocity
	 */
	vector_t get_data() {
		vector_t ans;
		ans[0] = m_a;
		ans[1] = m_a+1.0f;
		ans[2] = m_a+2.0f;
		m_a+=0.01f;
		return ans;
	}

	bool is_data_losed() { return false; }

private:
	float m_a;
};

int main(int argc, char** argv) {

	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " <server ip>" << std::endl;	
		return 1;
	}

	SimpleGen gen;
	VecWatch watch(&gen, argv[1], "test", 0.0f, 36.4f);

	watch.run();

	while (true) {
		watch.get_data();
		usleep(10000);
	}
	return 0;
}
