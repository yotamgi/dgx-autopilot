#include "util/watch.h"
#include "common/types.h"

class SimpleGen : public DataGenerator<vector_t> {
public:

	virtual ~SimpleGen(){}

	/**
	 * Return the angular velocity
	 */
	boost::shared_ptr<vector_t> get_data() {
		boost::shared_ptr<vector_t> ans(new vector_t);
		ans->x = 1.0;
		ans->y = 1.0;
		ans->z = 1.0;
		return ans;
	}

	bool is_data_losed() { return false; }

};
int main(int argc, char** argv) {

	SimpleGen gen;
	gen.get_data();
	VecWatch watch(&gen, "localhost");
	watch.run();
	return 0;
}
