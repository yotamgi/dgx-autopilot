#include <iostream>

#include <stream/data_pop_stream.h>
#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include "stream_watch/stream_presenter_3d.h"

class SimpleGen : public stream::DataPopStream<lin_algebra::vec3f> {
public:

	SimpleGen():m_a(0.0f) {}
	virtual ~SimpleGen(){}

	/**
	 * Return the angular velocity
	 */
	lin_algebra::vec3f get_data() {
		lin_algebra::vec3f ans;
		ans[0] = m_a; ans[1] = m_a+1.0f; ans[2] = m_a+2.0f;
		m_a+=0.01f;
		return ans;
	}

	bool is_data_losed() { return false; }

private:
	float m_a;
};

int main(int argc, char** argv) {

	boost::shared_ptr<SimpleGen> gen(new SimpleGen);

	if (argc == 1) {
		StreamPresenter s;
		s.addAngleStream(gen);
		s.run(false);
	} else if (argc == 2 && std::string(argv[1]) == std::string("--net")) {
		boost::shared_ptr<stream::TcpipClient> client =
				boost::make_shared<stream::TcpipClient>("localhost", 0x6060);
		stream::StreamConnection conn(client);
		conn.export_pop_stream<lin_algebra::vec3f>(gen, "dummy");
		std::cout << "Running..." << std::endl;
		conn.run(false);
	} else {
		std::cout <<"usage: " << argv[0] << " [--net] " << std::endl;
		exit(1);
	}

	return 0;
}
