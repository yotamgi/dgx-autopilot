#ifndef MAPSTREAMVIEW_H_
#define MAPSTREAMVIEW_H_

#include <QtGui>
#include <qgis/qgsmapcanvas.h>

namespace gs {

class MapStreamView : public QWidget {
public:

	MapStreamView(std::string map_fname, std::string map_type);

private:
	QgsMapCanvas* m_map_canvas;
};

}; // namespace gs

#endif /* MAPSTREAMVIEW_H_ */
