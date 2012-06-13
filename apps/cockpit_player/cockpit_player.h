#ifndef GROUND_STATION_H_
#define GROUND_STATION_H_

#include <string>
#include <stream/stream_utils.h>
#include <stream/util/stream_player.h>
#include <autopilot/cockpit.h>
#include <boost/shared_ptr.hpp>
#include <qapplication.h>
#include <QObject>
#include <qt4/Qt/qwidget.h>
#include <qt4/QtGui/qslider.h>
#include <qt4/QtGui/qlabel.h>


class CockpitPlayer : public QObject {
	Q_OBJECT
public:

	CockpitPlayer(std::string plane_addr);
	~CockpitPlayer(){}

	void run();

signals:


public slots:

	void play(bool state);
	void pause();
	void stop();
	void seek(int pos); // out of PROGRESS_RESOLUTION  field

protected:

	void timerEvent(QTimerEvent *);

private:

	static const size_t PROGRESS_RESOLUTION = 1000;

	typedef stream::PopStreamPlayer<lin_algebra::vec3f> vec3_pop_player;
	typedef stream::PushStreamPlayer<lin_algebra::vec3f> vec3_push_player;
	typedef stream::PushStreamPlayer<float> float_push_player;

	void update_cockpit();

	QApplication m_app;

	void start_players();
	void pause_players();
	void stop_players();
	void seek_players(float seek_t);

    boost::shared_ptr<vec3_pop_player> 		m_acc_sensor_player;
    boost::shared_ptr<vec3_pop_player> 		m_gyro_sensor_player;
    boost::shared_ptr<vec3_pop_player> 		m_compass_sensor_player;
    boost::shared_ptr<vec3_push_player> 	m_gps_pos_generator_player;
    boost::shared_ptr<float_push_player> 	m_gps_speed_dir_generator_player;
    boost::shared_ptr<float_push_player> 	m_gps_speed_mag_generator_player;

    boost::shared_ptr<autopilot::Cockpit> m_cockpit;

    boost::thread m_update_cockpit_thread;

    QAction* m_play_action;
    QSlider* m_progress_slider;
    QLabel* m_progress_text;
};

#endif /* GROUND_STATION_H_ */
