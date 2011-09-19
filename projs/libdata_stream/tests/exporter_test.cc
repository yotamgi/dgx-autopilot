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
