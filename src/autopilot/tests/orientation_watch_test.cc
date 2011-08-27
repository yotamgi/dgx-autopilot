#include "util/watch.h"
#include "common/types.h"
#include "hw/itg3200_gyro.h"

class IntergralFilter : public DataFilter<vector_t> {
public:
	IntergralFilter(DataGenerator<vector_t>* data_gen):
	DataFilter(data_gen) {
		sum.x = 1.f;
		sum.y = 1.f;
		sum.z = 1.f;
	}

	virtual ~IntergralFilter() {}

	boost::shared_ptr<vector_t> get_data() {
		boost::shared_ptr<vector_t> data = m_generator->get_data();
		sum.x += data->x/11.f;
		sum.y += data->y/11.f;
		sum.z += data->z/11.f;
		if (sum.x > 41111) sum.x = -4111;
		if (sum.x < -41111) sum.x = 4111;
		if (sum.y > 41111) sum.y = -4111;
		if (sum.y < -41111) sum.y = 4111;
		if (sum.z > 41111) sum.z = -4111;
		if (sum.z < -41111) sum.z = 4111;
		boost::shared_ptr<vector_t> ans(new vector_t);
		ans->x = sum.x;
		ans->y = sum.y;
		ans->z = sum.z;
		return  ans;
	}

private:
	vector_t sum;

};

int main(int argc, char** argv) {

	Itg3200Gyro gen(2);
	IntergralFilter integ(&gen);
	VecWatch watch(&integ, "192.168.0.19");

	watch.run();

	while (true) {
		watch.get_data();
		usleep(10000);
	}
	return 0;
}
