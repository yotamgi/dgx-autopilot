#include <algorithm>
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include "generators.h"
#include "stream_exporter.h"
#include "stream_importer.h"

using namespace stream;

class DummyIntStream : public DataGenerator<int> {
public:
	DummyIntStream():m_counter(0){}
	virtual ~DummyIntStream() {}

	int get_data() {
		return m_counter++;
	}

	bool is_data_losed() { return false; }

private:
	int m_counter;
};

class DummyVecStream : public VecGenerator<float, 3> {
public:
	DummyVecStream():m_counter(0){}
	virtual ~DummyVecStream() {}

	typename VecGenerator<float, 3>::vector_t get_data() {
		typename VecGenerator<float, 3>::vector_t v;
		v[0] = (float)m_counter;
		v[1] = (float)m_counter+1.;
		v[2] = (float)m_counter+2.;
		m_counter++;
		return v;
	}

	bool is_data_losed() { return false; }

private:
	int m_counter;
};


TEST(stream_export_import, int_stream) {

	// the stream we are about to send
	boost::shared_ptr<DataGenerator<int> > is(new DummyIntStream);

	// exporting the stream
	StreamExporter exporter;
	exporter.register_stream(is, "dummy_int");
	boost::thread exporter_thread(&StreamExporter::run, &exporter);

	// importing the stream
	StreamImporter importer("localhost");

	// checking that the data is correct
	boost::shared_ptr< DataGenerator<int> > a = importer.import_stream< DataGenerator<int> >("dummy_int");
	ASSERT_EQ(a->get_data(), 0);
	ASSERT_EQ(a->get_data(), 1);
	ASSERT_EQ(a->get_data(), 2);

	// kill the exporter.
	exporter.stop();
	exporter_thread.join();
}

TEST(stream_export_import, vec_stream) {

	// the stream we are about to send
	boost::shared_ptr<DataGenerator<VecGenerator<float, 3>::vector_t> > vs(new DummyVecStream);

	// exporting the stream
	StreamExporter exporter;
	exporter.register_stream(vs, "dummy_vec");
	boost::thread exporter_thread(&StreamExporter::run, &exporter);

	// importing the stream
	StreamImporter importer("localhost");

	// checking that the data is correct
	boost::shared_ptr< DataGenerator<DummyVecStream::vector_t> > a =
			importer.import_stream<DummyVecStream>("dummy_vec");

	ASSERT_NEAR(a->get_data()[2], 2, 0.01);
	sleep(1);
	ASSERT_NEAR(a->get_data()[1], 2, 0.01);
	ASSERT_NEAR(a->get_data()[0], 2, 0.01);


	// kill the exporter.
	exporter.stop();
	exporter_thread.join();
}

TEST(stream_export_import, list) {

	// the stream we are about to send
	boost::shared_ptr<DataGenerator<VecGenerator<float, 3>::vector_t> > vs(new DummyVecStream);

	// exporting the stream
	StreamExporter exporter;
	exporter.register_stream(vs, "dummy_vec0");
	exporter.register_stream(vs, "dummy_vec1");
	exporter.register_stream(vs, "dummy_vec2");
	boost::thread exporter_thread(&StreamExporter::run, &exporter);

	// importing the stream
	StreamImporter importer("localhost");

	// getting the streams from the importer
	std::vector<std::string> avail = importer.list_avail();

	ASSERT_EQ(std::count(avail.begin(), avail.end(), std::string("dummy_vec0")), 1);
	ASSERT_EQ(std::count(avail.begin(), avail.end(), std::string("dummy_vec1")), 1);
	ASSERT_EQ(std::count(avail.begin(), avail.end(), std::string("dummy_vec2")), 1);
}
