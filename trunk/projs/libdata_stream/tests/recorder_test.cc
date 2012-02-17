#include <stream/filters/stream_recorder.h>
#include <stream/data_pop_stream.h>
#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

class RunningGen : public stream::DataPopStream<int> {
public:
	RunningGen():m_i(0) {}
	virtual ~RunningGen() {}

	int get_data() {
		return m_i++;
	}
private:
	int m_i;
};

TEST(recorder, functional) {
	boost::shared_ptr<RunningGen> gen(new RunningGen);
	std::stringstream ss;

	stream::filters::RecorderPopFilter<int> filter(ss, gen);
	filter.get_data();
	filter.get_data();
	filter.get_data();

	std::cout << ss.str();
}
