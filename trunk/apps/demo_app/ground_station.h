#ifndef GROUND_STATION_H_
#define GROUND_STATION_H_

#include <string>
#include <stream/async_stream_connection.h>
#include <stream/util/tcpip_connection.h>
#include <stream/stream_utils.h>
#include <gs/size_push_gen.h>
#include <boost/shared_ptr.hpp>
#include <QtGui/QtGui>
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
	void use_airspeed(bool use);
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

	// some widgets that are needed during the class work
	gs::SizePushGen* m_np_tilt_control_widget;
	gs::SizePushGen* m_np_roll_control_widget;
	gs::SizePushGen* m_np_gas_control_widget;
	gs::SizePushGen* m_np_yaw_control_widget;
	gs::SizePushGen* m_sa_tilt_control_widget;
	gs::SizePushGen* m_sa_roll_control_widget;
	gs::SizePushGen* m_sa_gas_control_widget;
    gs::SizePushGen* m_sa_yaw_control_widget;

    QDoubleSpinBox* m_airspeed_value;

	boost::shared_ptr<stream::AsyncStreamConnection> m_connection;
	boost::shared_ptr<stream::PushToPopConv<float> > m_wanted_alt;
	stream::TcpipServer m_control_connection;
	KeyboardGetterWidget* m_keyboard_grabber;
	QApplication m_app;
};

#endif /* GROUND_STATION_H_ */
