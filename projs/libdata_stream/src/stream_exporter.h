
#ifndef STREAM_SEVER_H_
#define STREAM_SEVER_H_

#include <string>
#include <sstream>
#include <stdint.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include "generators.h"

class StreamExporter {
public:
	StreamExporter();

	template <typename T>
	void register_stream(DataGenerator<T>* stream, std::string stream_name) {
		m_exported_streams[stream_name] =
			boost::shared_ptr<AnyStream>(new SpecificStream<T>(stream)
		);
	}

	void run();

private:

	class AnyStream {
	public:
		virtual void serialize(std::ostream&);
	};

	template <typename T>
	class SpecificStream {
	public:
		SpecificStream(DataGenerator<T>* gen):m_gen(gen) {}

		void serialize(std::ostream& os) {
			os << m_gen->get_data();
		}
	private:
		DataGenerator<T>* m_gen;
	};

	void handle_client(int sock);

	std::map<std::string, boost::shared_ptr<AnyStream> > m_exported_streams;

	const uint32_t PORT;
};


#endif /* STREAM_SEVER_H_ */
