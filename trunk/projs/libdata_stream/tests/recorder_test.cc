#include <stream/util/stream_player.h>
#include <stream/filters/stream_recorder.h>
#include <stream/data_pop_stream.h>
#include <stream/stream_utils.h>
#include <stream/util/time.h>
#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <fstream>

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

TEST(player_recorder, blocking_functional) {
	boost::shared_ptr<RunningGen> gen(new RunningGen);
	std::stringstream ss;

	stream::filters::RecorderPopFilter<int> filter(ss, gen);

	// make the recording
	for (size_t i=0; i<100; i++) {
		usleep(10000); // 1/100 sec
		filter.get_data();
	}

	// play it
	stream::PopStreamPlayer<int> player(ss);
	Timer t;
	for (size_t i=0; i<100; i++) {
		ASSERT_EQ(i, player.get_data());
	}
	ASSERT_NEAR(t.passed(), 1.0, 0.2);
}

TEST(player_recorder, non_blocking_slow_fast) {
	boost::shared_ptr<RunningGen> gen(new RunningGen);
	std::stringstream ss;

	stream::filters::RecorderPopFilter<int> filter(ss, gen);

	// make the recording
	for (size_t i=0; i<100; i++) {
		usleep(10000); // 1/100 sec
		filter.get_data();
	}

	// play it
	stream::PopStreamPlayer<int> player(ss, false);
	Timer t;
	while (t.passed() < 0.9) {
		ASSERT_NEAR((int)(t.passed()*100), player.get_data(), 10);
	}
}

TEST(player_recorder, non_blocking_fast_slow) {
	boost::shared_ptr<RunningGen> gen(new RunningGen);
	std::stringstream ss;

	stream::filters::RecorderPopFilter<int> filter(ss, gen);

	// make the recording
	Timer t;
	while (t.passed() < 1.) {
		usleep(1000); // 1/1000
		filter.get_data();
	}
	int max = filter.get_data();

	// play it
	stream::PopStreamPlayer<int> player(ss, false);
	t.reset();
	while (t.passed() < 0.9) {
		usleep(10000); // 1/100 sec
		ASSERT_NEAR((int)(t.passed()*max), player.get_data(), max/10);
	}
}

TEST(player_recorder, blocking_functional_file) {
	boost::shared_ptr<RunningGen> gen(new RunningGen);
	//std::stringstream ss;
	std::ofstream record_file("/tmp/file");

	stream::filters::RecorderPopFilter<int> filter(record_file, gen);

	// make the recording
	for (size_t i=0; i<100; i++) {
		usleep(10000); // 1/100 sec
		filter.get_data();
	}
	record_file.close();

	// play it
	std::ifstream play_file("/tmp/file");
	stream::PopStreamPlayer<int> player(play_file);
	Timer t;
	for (size_t i=0; i<100; i++) {
		ASSERT_EQ(i, player.get_data());
	}
	ASSERT_NEAR(t.passed(), 1.0, 0.2);
}


TEST(player_recorder, push_functional) {
	boost::shared_ptr<stream::PushToPopConv<float> > stream(new stream::PushToPopConv<float>(0.));

	// record
	std::stringstream ss;
	stream::filters::RecorderPushFilter<float> filter(ss, stream);

	Timer t;
	while (t.passed() < 1.1) {
		filter.set_data(t.passed());
		usleep(100000); // 1/10 sec
	}

	// play
	stream::PushStreamPlayer<float> player(ss);
	player.set_receiver(stream);
	usleep(10000); // 1/100 sec

	t.reset();
	while (t.passed() < 0.9) {
		ASSERT_NEAR(stream->get_data(), t.passed(), 0.15);
	}

}

