#include "ground_station.h"
#include "command_protocol.h"

#include <stream/util/udpip_connection.h>
#include <gs/3d_stream_view.h>
#include <gs/size_stream_view.h>
#include <gs/map_stream_view.h>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

int stam;

GroundStation::GroundStation(std::string plane_address):
			m_control_connection(plane_address, commands::port),
			m_app(stam, (char**)0)
{
	std::cout << "Presenting UDP from addr " << plane_address << std::endl;

	//
	// Import all the streams
	//

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
	boost::shared_ptr<vec2_recv_stream> position = 					boost::make_shared<vec2_recv_stream>();

	// fill them into the recv_stream list
	stream::AsyncStreamConnection::recv_streams_t recv_streams = boost::assign::list_of
			((recv_stream_ptr)watch_compass_sensor   )
			((recv_stream_ptr)watch_acc_sensor       )
			((recv_stream_ptr)gyro_watch_orientation )
			((recv_stream_ptr)watch_rest_orientation )
			((recv_stream_ptr)orientation            )
	        ((recv_stream_ptr)reliability            )
	        ((recv_stream_ptr)gyro_fps               )
            ((recv_stream_ptr)position             	 );

	// creating the udp connection stuff
	boost::shared_ptr<stream::UdpipConnectionFactory> conn_factory =
			boost::make_shared<stream::UdpipConnectionFactory>(4444, plane_address, 5555);

	// create the async stream connection and run it
	m_connection = boost::make_shared<stream::AsyncStreamConnection>(
			stream::AsyncStreamConnection::send_streams_t(),
			recv_streams,
			conn_factory,
			true,
			50.);
	m_connection->start();

	//
	// Create the ground station window
	//

	// global parameters
	const QSize stream3d_dimention(400, 300);
	const float view_update_time = 0.01;

	gs::StreamView3d *view3d = new gs::StreamView3d(view_update_time, stream3d_dimention);

	// the right one
	view3d->addAngleStream(orientation, irr::core::vector3df(-20., 0., 0.));

	// the mid one
	view3d->addAngleStream(watch_rest_orientation, irr::core::vector3df(0., 0., 0.));
	view3d->addVecStream(watch_acc_sensor, irr::core::vector3df(0., 0., 0.));
	view3d->addVecStream(watch_compass_sensor, irr::core::vector3df(0., 0., 0.));

	// the right one
	view3d->addAngleStream(gyro_watch_orientation, irr::core::vector3df(20., 0., 0.));

	// the reliable stream
	gs::SizeStreamView *view_size = new gs::SizeStreamView(reliability, view_update_time, 0., 1.);

	// the gyro_fps stream
	gs::SizeStreamView *view_fps = new gs::SizeStreamView(gyro_fps, 1., 0., 4000.);

	// the position
	gs::MapStreamView* map_view = new gs::MapStreamView(position, 1.0f, stream3d_dimention,
					std::string("../../projs/ground_station/data/map"));

	// the link quality
	gs::SizeStreamView* view_link_quality = new gs::SizeStreamView(
			m_connection->get_quality_stream(), 0.1f , 0., 1.);

	// create the window itself
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(view_link_quality);
	layout->addWidget(view3d);
	layout->addWidget(view_size);
	layout->addWidget(view_fps);
	layout->addWidget(map_view);
	QWidget* wnd = new QWidget;
	wnd->setLayout(layout);
	wnd->show();
	view3d->start();
	view_size->start();
	view_fps->start();
	view_link_quality->start();

	// connect signals
	connect(map_view, SIGNAL(got_point(const QgsPoint&, Qt::MouseButton)),
			this, SLOT(got_waypoint(const QgsPoint&, Qt::MouseButton)));
}

void GroundStation::run() {
	m_app.exec();
};

void GroundStation::got_waypoint(const QgsPoint& geo_waypoint, Qt::MouseButton button) {
	std::cout << "GS: Got point!" << std::endl;
	boost::shared_ptr<stream::Connection> conn = m_control_connection.get_connection();

	lin_algebra::vec2f wp = lin_algebra::create_vec2f(geo_waypoint.x(), geo_waypoint.y());
	std::stringstream wp_str;
	wp_str << wp;

	conn->write(commands::NAVIGATE_TO);
	conn->write(wp_str.str());
}

GroundStation::~GroundStation() {}
