#include <iostream>
#include "stream_presenter.h"
#include <stream/stream_importer.h>

int main(int argc, char** argv) {

	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " <address> " << std::endl;
		exit(1);
	}
	std::string address = argv[1];

	stream::StreamImporter imp(address);
	std::vector<std::string> avail = imp.list_avail();

	StreamPresenter presenter;

	// some ui
	std::cout << "The available streams are: ";
	std::copy(avail.begin(), avail.end(), std::ostream_iterator<std::string>(std::cout, " "));
	std::cout << std::endl << "Which one do you want? ";
	std::cout.flush();
	int which;
	std::cin >> which;

	std::cout << "Do you want it as angle (a) or vec (v)? ";
	std::cout.flush();
	char c;
	std::cin >> c;

	boost::shared_ptr<stream::VecGenerator<float,3> > chosen =
			imp.import_stream<stream::VecGenerator<float,3> >(avail.at(which));

	//std::cout << chosen->get_data() << std::endl;

	if (c == 'a') {
		presenter.addAngleStream(chosen);
	} else if (c == 'v') {
		presenter.addVecStream(chosen);
	}

	presenter.run(false);

	return 0;
}
