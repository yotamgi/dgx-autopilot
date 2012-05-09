#ifndef GROUND_STATION_H_
#define GROUND_STATION_H_

#include <string>
#include <stream/async_stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <boost/shared_ptr.hpp>
#include <qapplication.h>
#include <QObject>
#include <qgis/qgspoint.h>

class GroundStation : public QObject {
	Q_OBJECT
public:

	GroundStation(std::string plane_addr);
	~GroundStation();

	void run();

signals:


public slots:

	void got_waypoint(const QgsPoint& geo_waypoint, Qt::MouseButton button);

private:

	boost::shared_ptr<stream::AsyncStreamConnection> m_connection;
	stream::TcpipServer m_control_connection;
	QApplication m_app;
};

#endif /* GROUND_STATION_H_ */
