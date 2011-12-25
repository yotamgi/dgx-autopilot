#ifndef MAPSTREAMVIEW_H_
#define MAPSTREAMVIEW_H_

#include <stream/data_pop_stream.h>
#include <stream/util/lin_algebra.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <QtGui/QtGui>
#include <qgis/qgsmapcanvas.h>
#include <boost/filesystem.hpp>

namespace gs {

typedef stream::DataPopStream<lin_algebra::vec2f> pos_stream;

class MapStreamView : public QWidget {
	Q_OBJECT
public:

	MapStreamView(boost::shared_ptr<pos_stream> pos_stream,
				  float update_time,
				  QSize widget_size,
				  std::string map_dir
	);

public slots:

	void update();

private:

	void load_map(boost::filesystem::path p);

	QgsMapCanvas* m_map_canvas;
	boost::shared_ptr<QgsRubberBand> m_plane_track;

	boost::shared_ptr<pos_stream> m_pos_stream;

	float m_update_time;

	QTimer* m_timer;
};

}; // namespace gs

#endif /* MAPSTREAMVIEW_H_ */
