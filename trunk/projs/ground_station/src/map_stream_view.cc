#include "map_stream_view.h"
#include <qgis/qgsproviderregistry.h>
#include <qgis/qgsvectorlayer.h>
#include <qgis/qgssinglesymbolrenderer.h>
#include <qgis/qgsmaplayerregistry.h>
#include <qgis/qgsrubberband.h>
#include <stdexcept>

namespace gs {

MapStreamView::MapStreamView(std::string map_fname,
		  	  	  	  	  	 std::string map_type,
		  	  	  	  	  	 boost::shared_ptr<pos_stream> pos_stream,
		  	  	  	  	  	 float update_time):
		  	 m_map_canvas(new QgsMapCanvas(0,0)),
		  	 m_pos_stream(pos_stream),
		  	 m_update_time(update_time)
{
	QgsProviderRegistry::instance("/usr/lib/qgis");

	// create and configure the map canvas
	m_map_canvas = new QgsMapCanvas();
	m_map_canvas->enableAntiAliasing(true);
	m_map_canvas->useImageToRender(false);
	m_map_canvas->setCanvasColor(QColor(255, 255, 255));
	m_map_canvas->freeze(false);
	m_map_canvas->setVisible(true);
	m_map_canvas->refresh();
	m_map_canvas->setFixedSize(QSize(300, 200));
	m_map_canvas->show();


	// create the vector layer
	QgsVectorLayer * map_layer = new QgsVectorLayer(QString(map_fname.c_str()), tr("layer1"), QString(map_type.c_str()));
	QgsSingleSymbolRenderer *map_renderer = new QgsSingleSymbolRenderer(map_layer->geometryType());
	QList<QgsMapCanvasLayer> map_layer_set;
	map_layer->setRenderer(map_renderer);
	if (!map_layer->isValid()) {
		throw std::runtime_error("Couldn't load the map layer");
	}
	// Add the Vector Layer to the Layer Registry
	QgsMapLayerRegistry::instance()->addMapLayer(map_layer, TRUE);

	map_layer_set.append(QgsMapCanvasLayer(map_layer));
	m_map_canvas->setExtent(map_layer->extent());
	m_map_canvas->setLayerSet(map_layer_set);


 	m_plane_track = boost::shared_ptr<QgsRubberBand>(new QgsRubberBand(m_map_canvas, false));
	m_map_canvas->scene()->addItem(m_plane_track.get());

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_map_canvas);
	setLayout(layout);

 	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer->start(1000*m_update_time);
}

void MapStreamView::update() {
	lin_algebra::vec2f pos = m_pos_stream->get_data();
	QgsPoint p(pos[0], pos[1]);

	// go to the current plane pos on the map
	std::cout << "Setting extent" << std::endl;
	m_map_canvas->setExtent(QgsRectangle(p, p));
	m_map_canvas->refresh();

	// draw the line of the plane's way
	m_plane_track->addPoint(p);
	m_map_canvas->refresh();
}

} // namespace gs
