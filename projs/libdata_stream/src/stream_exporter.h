
#ifndef STREAM_SEVER_H_
#define STREAM_SEVER_H_

#include <string>
#include <sstream>
#include <stdint.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include "generators.h"
#include "protocol.h"

namespace stream {

class StreamExporter {
public:
	StreamExporter();
	~StreamExporter();

	template <typename T>
	void register_stream(DataGenerator<T>* stream, std::string stream_name) {
		m_exported_streams[stream_name] =
			boost::shared_ptr<AnyStream>(new SpecificStream<T>(stream)
		);
	}

	/**
	 * Runs the server.
	 * This function will block.
	 */
	void run();

	/**
	 * Stops the server.
	 * Can be called asynchronusly while run function runs, and it will stop
	 * it.
	 */
	void stop();

private:

	class AnyStream {
	public:
		virtual void serialize(std::ostream&) = 0;
	};

	template <typename T>
	class SpecificStream : public AnyStream {
	public:
		SpecificStream(DataGenerator<T>* gen):m_gen(gen) {}

		void serialize(std::ostream& os) {
			os << m_gen->get_data();
		}
	private:
		DataGenerator<T>* m_gen;
	};

	void handle_client();

	volatile bool m_is_running;

	int m_client_sock;
    int m_server_sock;


	std::map<std::string, boost::shared_ptr<AnyStream> > m_exported_streams;
};

} // namespace stream

#endif /* STREAM_SEVER_H_ */
