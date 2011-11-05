#include <gtest/gtest.h>
#include "filters/static_filter.h"
#include "generators.h"

class StaticDummyGen : public stream::DataGenerator<lin_algebra::vector_t> {
public:

	lin_algebra::vector_t get_data() {
		lin_algebra::vector_t a;
		a[0] = 1.; a[1] = 2.; a[2] = 3.;
		return a;
	}

};

TEST(static_filter, functional) {
	boost::shared_ptr<StaticDummyGen> gen(new StaticDummyGen);

	lin_algebra::vector_t a; a[0] =-1.; a[1] =-2.; a[2] =-3.;

	stream::filters::StaticFilter<lin_algebra::vector_t> filter(gen, a);

	for (size_t i=0; i<100; i++) {
		lin_algebra::vector_t data = filter.get_data();

		for (size_t i=0; i<3; i++) {
			ASSERT_NEAR(data[i], 0., 0.01);
		}
	}
}
