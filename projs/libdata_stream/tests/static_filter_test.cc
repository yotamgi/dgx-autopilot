#include <gtest/gtest.h>
#include "filters/static_filter.h"
#include "generators.h"

class DummyGen : public stream::VecGenerator<float, 3> {
public:
	typedef typename VecGenerator<float, 3>::vector_t vector_t;

	vector_t get_data() {
		vector_t a;
		a[0] = 1.; a[1] = 2.; a[2] = 3.;
		return a;
	}
	virtual bool is_data_losed() {return false;};

};

TEST(static_filter, functional) {
	boost::shared_ptr<DummyGen> gen(new DummyGen);
	DummyGen::vector_t a; a[0] =-1.; a[1] =-2.; a[2] =-3.;
	DummyGen::vector_t b; b[0] = 1.; b[1] = 1.; b[2] = 1.;

	stream::filters::StaticFilter<float, 3> filter(gen, a, b);

	for (size_t i=0; i<100; i++) {
		DummyGen::vector_t data = filter.get_data();

		for (size_t i=0; i<3; i++) {
			ASSERT_NEAR(data[i], 0., 0.01);
		}
	}
}
