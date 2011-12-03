#include <iostream>
#include <boost/make_shared.hpp>
#include "stream_presenter.h"
#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <stream/util/lin_algebra.h>

int main(int argc, char** argv) {

	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " <address> " << std::endl;
		exit(1);
	}
	std::string address = argv[1];

	boost::shared_ptr<stream::TcpipServer> server =
			boost::make_shared<stream::TcpipServer>("localhost", 0x6060);
	stream::StreamConnection conn(server);
	conn.run();
	std::vector<std::string> avail = conn.list_avail();

	StreamPresenter presenter;

	// some ui
	while(true) {
		std::cout << "The available streams are: ";
		std::copy(avail.begin(), avail.end(), std::ostream_iterator<std::string>(std::cout, " "));
		std::cout << std::endl << "Which one do you want (or 0 to exit)? ";
		std::cout.flush();
		int which;
		std::cin >> which;
		if (which == 0) break;

		std::cout << "Do you want it as angle (a) or vec (v)? ";
		std::cout.flush();
		char c;
		std::cin >> c;

		boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > chosen =
				conn.import_pop_stream<lin_algebra::vec3f>(avail.at(which-1));

		//std::cout << chosen->get_data() << std::endl;

		if (c == 'a') {
			presenter.addAngleStream(chosen);
		} else if (c == 'v') {
			presenter.addVecStream(chosen);
		}
	}

	presenter.run(false);

	return 0;
}
