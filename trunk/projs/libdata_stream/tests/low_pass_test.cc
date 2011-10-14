#include <gtest/gtest.h>
#include <iostream>
#include <stream/generators.h>
#include <stream/filters/low_pass_filter.h>

class JitterGen : public stream::DataGenerator<float> {
public:
	JitterGen(float a, float b, size_t interval):
		m_a(a), m_b(b), m_times(interval), m_counter(0)
	{}
	~JitterGen() {}

	float get_data() {
		if (++m_counter == m_times) {
			m_counter = 0;
			return m_b;
		} else {
			return m_a;
		}
	}
private:
	const float m_a;
	const float m_b;
	const size_t m_times;
	size_t m_counter;
};

TEST(low_pass_filter, functional) {
	boost::shared_ptr<JitterGen> a(new JitterGen(0., 5., 5));

	stream::filters::LowPassFilter<float> lp(a, 5);
	for (int i=0; i<100000; i++) {
		ASSERT_NEAR(lp.get_data(), 1.0f, 0.0001f);
	}
}
