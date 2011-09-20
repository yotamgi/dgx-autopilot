#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include "generators.h"
#include "stream_exporter.h"
#include "stream_importer.h"

class DummyIntStream : public DataGenerator<int> {
public:
	DummyIntStream():m_counter(0){}

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

	typename VecGenerator<float, 3>::vector_t get_data() {
		typename VecGenerator<float, 3>::vector_t v;
		v[0] = (float)m_counter;
		v[0] = (float)m_counter+1.;
		v[0] = (float)m_counter+2.;
		m_counter++;
		return v;
	}

	bool is_data_losed() { return false; }

private:
	int m_counter;
};


TEST(stream_export_import, int_stream) {

	// the stream we are about to send
	DummyIntStream is;

	// exporting the stream
	std::cout << "Running exporter" << std::endl;
	StreamExporter exporter;
	exporter.register_stream(&is, "dummy_int");
	boost::thread exporter_thread(&StreamExporter::run, &exporter);

	// importing the stream
	StreamImporter importer("localhost");
	std::cout << "importer is connected" << std::endl;

	// checking that the data is correct
	boost::shared_ptr< DataGenerator<int> > a = importer.import_stream<int>("dummy_int");
	ASSERT_EQ(a->get_data(), 0);
	ASSERT_EQ(a->get_data(), 1);
	ASSERT_EQ(a->get_data(), 2);

	// kill the exporter.
	exporter_thread.detach();
}

TEST(stream_export_import, vec_stream) {

	// the stream we are about to send
	DummyVecStream vs;

	// exporting the stream
	std::cout << "Running exporter" << std::endl;
	StreamExporter exporter;
	exporter.register_stream(&vs, "dummy_vec");
	boost::thread exporter_thread(&StreamExporter::run, &exporter);

	// importing the stream
	StreamImporter importer("localhost");
	std::cout << "importer is connected" << std::endl;

	// checking that the data is correct
	boost::shared_ptr< DataGenerator<DummyVecStream::vector_t> > a =
			importer.import_stream<DummyVecStream::vector_t>("dummy_vec");

	ASSERT_NEAR(a->get_data()[2], 2, 0.01);
	ASSERT_NEAR(a->get_data()[1], 2, 0.01);
	ASSERT_NEAR(a->get_data()[0], 2, 0.01);

	// kill the exporter.
	exporter.stop();
	exporter_thread.join();
}
