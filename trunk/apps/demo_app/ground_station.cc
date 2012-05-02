#include <stream/stream_connection.h>
#include <stream/async_stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <stream/util/udpip_connection.h>
#include <gs/3d_stream_view.h>
#include <gs/size_stream_view.h>
#include <gs/map_stream_view.h>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>

#include <string>
#include <iostream>
#include <fstream>

void usage_and_exit(std::string arg0) {
	std::cout << arg0 << " <plane_address>  [--help] " << std::endl;
	exit(1);
}

typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec3f> > vec3_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPopStream<lin_algebra::vec2f> > vec2_pop_stream_ptr;
typedef boost::shared_ptr<stream::DataPopStream<float> > 			  float_pop_stream_ptr;

void open_gs(
		vec3_pop_stream_ptr compass_sensor,
		vec3_pop_stream_ptr acc_sensor,
		vec3_pop_stream_ptr gyro_orientation,
		vec3_pop_stream_ptr rest_orientation,
		vec3_pop_stream_ptr orientation,
		float_pop_stream_ptr reliability,
		float_pop_stream_ptr fps,
		vec2_pop_stream_ptr position,
		float_pop_stream_ptr link_quality = float_pop_stream_ptr()
	)
{

	// global parameters
	const QSize stream3d_dimention(400, 300);
	const float view_update_time = 0.01;

	int a=0;
	QApplication app(a, (char**)0);
	gs::StreamView3d view3d(view_update_time, stream3d_dimention);

	// the right one
	view3d.addAngleStream(orientation, irr::core::vector3df(-20., 0., 0.));

	// the mid one
	view3d.addAngleStream(rest_orientation, irr::core::vector3df(0., 0., 0.));
	view3d.addVecStream(acc_sensor, irr::core::vector3df(0., 0., 0.));
	view3d.addVecStream(compass_sensor, irr::core::vector3df(0., 0., 0.));

	// the right one
	view3d.addAngleStream(gyro_orientation, irr::core::vector3df(20., 0., 0.));

	// the reliable stream
	gs::SizeStreamView view_size(reliability, view_update_time, 0., 1.);

	// the gyro_fps stream
	gs::SizeStreamView view_fps(fps, 1., 0., 4000.);

	// the position
	gs::MapStreamView map_view(position, 1.0f, stream3d_dimention,
					std::string("../../projs/ground_station/data/map"));

	// the link quality
	gs::SizeStreamView view_link_quality(link_quality, 0.1f , 0., 1.);


	// create the window itself
	QHBoxLayout* layout = new QHBoxLayout();
	if (link_quality) layout->addWidget(&view_link_quality);
	layout->addWidget(&view3d);
	layout->addWidget(&view_size);
	if (fps) layout->addWidget(&view_fps);
	layout->addWidget(&map_view);
	QWidget* wnd = new QWidget;
	wnd->setLayout(layout);
	wnd->show();
	view3d.start();
	view_size.start();
	if (fps) view_fps.start();
	if (link_quality) view_link_quality.start();
	app.exec();
}

int main(int argc, char** argv) {
	// commandline parsing
	std::string plane_address;

	if (argc != 2)							usage_and_exit(argv[0]);
	if (std::string(argv[1]) == "--help")	usage_and_exit(argv[0]); 

	plane_address = argv[1];

	std::cout << "Presenting UDP from addr " << plane_address << std::endl;

	typedef boost::shared_ptr<stream::AsyncStreamConnection::RecvStream> recv_stream_ptr;
	typedef stream::AsyncStreamConnection::RecvPopStream<lin_algebra::vec3f> vec3_recv_stream;
	typedef stream::AsyncStreamConnection::RecvPopStream<lin_algebra::vec2f> vec2_recv_stream;
	typedef stream::AsyncStreamConnection::RecvPopStream<float> float_recv_stream;

	// create the streams
	boost::shared_ptr<vec3_recv_stream> watch_compass_sensor = 		boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<vec3_recv_stream> watch_acc_sensor = 			boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<vec3_recv_stream> gyro_watch_orientation =	boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<vec3_recv_stream> watch_rest_orientation =	boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<vec3_recv_stream> orientation = 				boost::make_shared<vec3_recv_stream>();
	boost::shared_ptr<float_recv_stream> reliability = 				boost::make_shared<float_recv_stream>();
	boost::shared_ptr<float_recv_stream> gyro_fps = 				boost::make_shared<float_recv_stream>();
	boost::shared_ptr<vec2_recv_stream> posisition = 				boost::make_shared<vec2_recv_stream>();

	// fill them into the recv_stream list
	stream::AsyncStreamConnection::recv_streams_t recv_streams = boost::assign::list_of
			((recv_stream_ptr)watch_compass_sensor   )
			((recv_stream_ptr)watch_acc_sensor       )
			((recv_stream_ptr)gyro_watch_orientation )
			((recv_stream_ptr)watch_rest_orientation )
			((recv_stream_ptr)orientation            )
	        ((recv_stream_ptr)reliability            )
	        ((recv_stream_ptr)gyro_fps               )
            ((recv_stream_ptr)posisition             );

	// creating the udp connection stuff
	boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
			boost::make_shared<stream::UdpipConnectionFactory>(4444, plane_address, 5555);

	// create the async stream connection
	stream::AsyncStreamConnection c(stream::AsyncStreamConnection::send_streams_t(),
									recv_streams,
									conn_factory,
									true,
									50.);
	c.start();

	// open the ground station with the streams
	open_gs(
			watch_compass_sensor,
			watch_acc_sensor,
			gyro_watch_orientation,
			watch_rest_orientation,
			orientation,
			reliability,
			gyro_fps,
			posisition,
			c.get_quality_stream()
	);
	return 0;

}
