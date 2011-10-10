
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

	template <typename Stream>
	void register_stream(boost::shared_ptr<Stream> stream, std::string stream_name) {
		m_exported_streams[stream_name] =
			boost::shared_ptr<AnyStream>(new SpecificStream<typename Stream::type>(stream)
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
		SpecificStream(boost::shared_ptr<DataGenerator<T> > gen):m_gen(gen) {}

		void serialize(std::ostream& os) {
			os << m_gen->get_data();
		}
	private:
		boost::shared_ptr<DataGenerator<T> > m_gen;
	};

	typedef std::map<std::string, boost::shared_ptr<AnyStream> > stream_map_t;

	void handle_client();

	volatile bool m_is_running;

	int m_client_sock;
    int m_server_sock;


	stream_map_t m_exported_streams;
};

} // namespace stream

#endif /* STREAM_SEVER_H_ */
