#include <stream/stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <platform/dgx1_platform.h>
#include <cockpit.h>
#include <gs/3d_stream_view.h>
#include <gs/size_stream_view.h>
#include <gs/map_stream_view.h>

#include <string>
#include <iostream>

void usage(std::string arg0) {
	std::cout << arg0 << " [--sensor-sim <address> ]  [--present-from <address> ] [--present-local]  [--help]" << std::endl;
}

void update_cockpit(autopilot::Cockpit* cockpit) {
	while (true) {
		cockpit->orientation()->get_data();
	}
}

int main(int argc, char** argv) {
	// global parameters
	const QSize stream3d_dimention(640, 480);
	const float view_update_time = 0.01;

	// commandline parsing
	bool sim = false;
	std::string sim_addr;
	std::string present_addr;
	bool present_local = false;
	if (argc > 2) {
		for (size_t i=1; i<(size_t)argc; i++) {

			if (std::string(argv[i]) == "--sensor-sim") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(1);
				} else {
					sim_addr = argv[i+1];
					i++;
				}
				sim = true;
				continue;
			}
			else if (std::string(argv[i]) == "--present-local") {
				present_local = true;
				continue;
			}
			else if (std::string(argv[i]) == "--present-from") {
				if ((size_t)argc < i+1) {
					usage(argv[0]);
					exit(2);
				} else {
					present_addr = argv[i+1];
					i++;
				}
				continue;
			}
			else {
				usage(argv[0]);
				exit(1);
			}

		}
	} else if (argc != 1) {
		usage(argv[0]);
		exit(1);
	}

	if (present_addr != "")  {
		std::cout << "Presenting from addr " << present_addr << std::endl;
		QApplication app(argc, argv);
		gs::StreamView3d view3d(view_update_time, stream3d_dimention);

		boost::shared_ptr<stream::TcpipServer> server = boost::make_shared<stream::TcpipServer>(present_addr, 0x6060);
		stream::StreamConnection conn(server);

		typedef stream::DataPopStream<lin_algebra::vec3f> vs;
		typedef stream::DataPopStream<lin_algebra::vec2f> v2s;
		typedef stream::DataPopStream<float> fs;

		// the left one
		view3d.addAngleStream(conn.import_pop_stream<vs::type>("orientation"), irr::core::vector3df(-20., 0., 0.));

		// the mid one
		view3d.addAngleStream(conn.import_pop_stream<vs::type>("watch_rest_orientation"), irr::core::vector3df(0., 0., 0.));
		view3d.addVecStream(conn.import_pop_stream<vs::type>("watch_acc_sensor"), irr::core::vector3df(0., 0., 0.));
		view3d.addVecStream(conn.import_pop_stream<vs::type>("watch_compass_sensor"), irr::core::vector3df(0., 0., 0.));

		// the right one
		view3d.addAngleStream(conn.import_pop_stream<vs::type>("gyro_watch_orientation"), irr::core::vector3df(20., 0., 0.));

		// the reliable stream
		gs::SizeStreamView view_size(conn.import_pop_stream<fs::type>("reliability"), view_update_time, 0., 1.);

		// the position
		gs::MapStreamView map_view(conn.import_pop_stream<v2s::type>("position"), 1.0f, stream3d_dimention,
				std::string("../ground_station/data/map"));

		// create the window itself
		QHBoxLayout* layout = new QHBoxLayout();
		layout->addWidget(&view3d);
		layout->addWidget(&view_size);
		layout->addWidget(&map_view);
		QWidget* wnd = new QWidget;
		wnd->setLayout(layout);
		wnd->show();
		view3d.start();
		view_size.start();
		app.exec();

	} else {


		autopilot::NormalPlainPlatform platform =
			sim?
				autopilot::create_dgx1_simulator_platform(boost::make_shared<stream::TcpipServer>(sim_addr, 0x6060)):
				autopilot::create_dgx1_platform();

		autopilot::Cockpit cockpit(platform);

		boost::thread update_thread(update_cockpit, &cockpit);

		if (present_local) {
			std::cout << "Presenting locally" << std::endl;

			QApplication app(argc, argv);
			gs::StreamView3d view3d(view_update_time, stream3d_dimention);

			// the left one
			view3d.addAngleStream(cockpit.orientation()->get_watch_stream(), irr::core::vector3df(-20., 0., 0.));

			// the mid one
			view3d.addAngleStream(cockpit.watch_rest_orientation(), irr::core::vector3df(0., 0., 0.));
			view3d.addVecStream(cockpit.watch_fixed_acc(), irr::core::vector3df(0., 0., 0.));
			view3d.addVecStream(platform.compass_sensor->get_watch_stream(), irr::core::vector3df(0., 0., 0.));

			// the right one
			view3d.addAngleStream(cockpit.watch_gyro_orientation(), irr::core::vector3df(20., 0., 0.));

			// the reliable stream
			gs::SizeStreamView view_size(cockpit.watch_rest_reliability(), view_update_time, 0., 1.);

			// the position
			gs::MapStreamView map_view(cockpit.position(), 1.0f, stream3d_dimention,
							std::string("../ground_station/data/map"));


			// create the window itself
			QHBoxLayout* layout = new QHBoxLayout();
			layout->addWidget(&view3d);
			layout->addWidget(&view_size);
			layout->addWidget(&map_view);
			QWidget* wnd = new QWidget;
			wnd->setLayout(layout);
			wnd->show();
			view3d.start();
			view_size.start();
			app.exec();

		}  else {
			std::cout << "Exporting all data" << std::endl;
			boost::shared_ptr<stream::TcpipClient> client = boost::make_shared<stream::TcpipClient>(present_addr, 0x6060);
			stream::StreamConnection conn(client);
			conn.export_pop_stream<lin_algebra::vec3f>(cockpit.watch_gyro_orientation(), "gyro_watch_orientation");
			conn.export_pop_stream<lin_algebra::vec3f>(cockpit.watch_rest_orientation(), "watch_rest_orientation");
			conn.export_pop_stream<lin_algebra::vec3f>(cockpit.watch_fixed_acc(), "watch_acc_sensor");
			conn.export_pop_stream<lin_algebra::vec3f>(platform.compass_sensor->get_watch_stream(), "watch_compass_sensor");
			conn.export_pop_stream<lin_algebra::vec3f>(cockpit.orientation(), "orientation");
			conn.export_pop_stream<float>(cockpit.watch_rest_reliability(), "reliability");
			conn.export_pop_stream<lin_algebra::vec2f>(cockpit.position(), "position");

			// run connection and make it block
			conn.run(false);
		}

	}
	return 0;

}
