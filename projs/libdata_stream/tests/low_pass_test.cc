#include <gtest/gtest.h>
#include <iostream>
#include <stream/data_pop_stream.h>
#include <stream/filters/low_pass_filter.h>
#include <stream/util/lin_algebra.h>

class JitterGen : public stream::DataPopStream<float> {
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

class JitterVecGen : public stream::DataPopStream<lin_algebra::vec3f> {
public:
	JitterVecGen(float a, float b):
		m_a(a), m_b(b), m_counter(0)
	{}
	~JitterVecGen() {}

	lin_algebra::vec3f get_data() {
		lin_algebra::vec3f ans;
		for (size_t i=0; i<3; i++) {
			ans[i] = (m_counter == i)?m_b:m_a;
		}
		if (++m_counter == 3) m_counter = 0;

		return ans;
	}
private:
	const float m_a;
	const float m_b;
	size_t m_counter;
};


TEST(low_pass_filter, functional) {
	boost::shared_ptr<JitterGen> a(new JitterGen(0., 5., 5));

	stream::filters::LowPassFilter<float> lp(a, 5);
	for (int i=0; i<100000; i++) {
		ASSERT_NEAR(lp.get_data(), 1.0f, 0.0001f);
	}
}

TEST(low_pass_filter, vec_functional) {
	boost::shared_ptr<JitterVecGen> a(new JitterVecGen(0., 3.));

	stream::filters::LowPassFilter<lin_algebra::vec3f> lp(a, 3);
	for (int i=0; i<100000; i++) {
		lin_algebra::vec3f dat = lp.get_data();
		for (int i=0; i<3; i++)
			ASSERT_NEAR(dat[i], 1.0f, 0.0001f);
	}
}
