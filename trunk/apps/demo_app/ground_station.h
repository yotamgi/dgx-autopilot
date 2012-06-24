#ifndef GROUND_STATION_H_
#define GROUND_STATION_H_

#include <string>
#include <stream/async_stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <stream/stream_utils.h>
#include <gs/size_push_gen.h>
#include <boost/shared_ptr.hpp>
#include <qapplication.h>
#include <QObject>
#include <qt4/Qt/qwidget.h>
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
	void to_waypoint_pilot();
	void to_no_pilot();
	void to_sa_pilot();
	void calibrate();

private:

	class KeyboardGetterWidget : public QWidget  {
	public:
		KeyboardGetterWidget(gs::SizePushGen* pitch,
							 gs::SizePushGen* roll);
	protected:
		void keyPressEvent(QKeyEvent *k);
		void keyReleaseEvent(QKeyEvent *k);
	private:
		gs::SizePushGen* m_pitch;
		gs::SizePushGen* m_roll;
		float m_last_pitch;
		float m_last_roll;
	};

	boost::shared_ptr<stream::AsyncStreamConnection> m_connection;
	boost::shared_ptr<stream::PushToPopConv<float> > m_wanted_alt;
	stream::TcpipServer m_control_connection;
	KeyboardGetterWidget* m_keyboard_grabber;
	QApplication m_app;
};

#endif /* GROUND_STATION_H_ */
