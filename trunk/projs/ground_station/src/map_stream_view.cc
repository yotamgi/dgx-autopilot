#include "map_stream_view.h"
#include <qgis/qgsproviderregistry.h>
#include <qgis/qgsvectorlayer.h>
#include <qgis/qgssinglesymbolrenderer.h>
#include <qgis/qgsmaplayerregistry.h>
#include <stdexcept>

namespace gs {

MapStreamView::MapStreamView(std::string map_fname, std::string map_type):
		m_map_canvas(new QgsMapCanvas(0,0))
{
	QgsProviderRegistry::instance("/usr/lib/qgis");

	// create and configure the map canvas
	m_map_canvas = new QgsMapCanvas(0, 0);
	m_map_canvas->enableAntiAliasing(true);
	m_map_canvas->useImageToRender(false);
	m_map_canvas->setCanvasColor(QColor(255, 255, 255));
	m_map_canvas->freeze(false);
	m_map_canvas->setVisible(true);
	m_map_canvas->refresh();
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

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_map_canvas);
	setLayout(layout);
}

} // namespace gs
