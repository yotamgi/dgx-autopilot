#include "map_stream_view.h"
#include <qgis/qgsproviderregistry.h>
#include <qgis/qgsvectorlayer.h>
#include <qgis/qgssinglesymbolrenderer.h>
#include <qgis/qgsmaplayerregistry.h>
#include <stdexcept>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace gs {

MapStreamView::MapStreamView(boost::shared_ptr<pos_stream> pos_stream,
		  	  	  	  	  	 float update_time,
		  	  	  	  	  	 QSize widget_size,
		  	  	  	  	  	 std::string map_dir):
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
	m_map_canvas->setFixedSize(widget_size);
	m_map_canvas->show();

	load_map(fs::path(map_dir));

 	m_plane_track = new QgsRubberBand(m_map_canvas, false);
 	m_plane_track->setColor(QColor(255, 0, 0, 255));
 	m_plane_track->setWidth(3);
	m_map_canvas->scene()->addItem(m_plane_track);

	m_emit = new QgsMapToolEmitPoint(m_map_canvas);
	m_map_canvas->setMapTool(m_emit);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_map_canvas);
	setLayout(layout);

 	m_timer = new QTimer(this);
 	connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer->start(1000*m_update_time);

	m_emit->activate();
	connect(m_emit, SIGNAL(canvasClicked(const QgsPoint &,Qt::MouseButton)),
			this, SLOT(clicked(const QgsPoint &, Qt::MouseButton)));
}

MapStreamView::~MapStreamView() {
	delete m_map_canvas;
	delete m_plane_track;
	for (size_t i=0; i<m_dots.size(); i++) {
		delete m_dots[i];
	}
}

void MapStreamView::clicked(const QgsPoint & p, Qt::MouseButton button) {
	emit got_point(p , 	button);
	std::cout << "Clicked " << p.x() <<  ", " << p.y() << std::endl;

	QgsVertexMarker* dot = new QgsVertexMarker(m_map_canvas);
// 	dot->setColor(QColor(255, 0, 0, 255));
// 	dot->track->setWidth(3);
	dot->setCenter(p);
	m_map_canvas->scene()->addItem(dot);
	m_dots.push_back(dot);
}

void MapStreamView::load_map(fs::path dir) {
	// validate that it is a dir
	if (!fs::exists(dir) || !fs::is_directory(dir)) {
		throw std::runtime_error("The map dir path was not a directory");
	}

	// create the layer set, where we will put all the layers
	QList<QgsMapCanvasLayer> map_layer_set;

	// iterate over the files in the directory, and fill the layer set
	for (fs::directory_iterator f(dir); f != fs::directory_iterator(); f++) {
		std::string extention = f->path().extension().c_str();

		// if it is a vector layer
		if (extention == ".shp") {
			std::cout << "added shp layer " << f->path().filename().c_str() << std::endl;
			// create the vector layer
			QgsVectorLayer* map_layer = new QgsVectorLayer(tr(f->path().c_str()), tr(f->path().filename().c_str()), QString("ogr"));
			QgsSingleSymbolRenderer *map_renderer = new QgsSingleSymbolRenderer(map_layer->geometryType());
			map_layer->setRenderer(map_renderer);
			if (!map_layer->isValid()) {
				throw std::runtime_error("Couldn't load the map layer");
			}
			// Add the Vector Layer to the Layer Registry
			QgsMapLayerRegistry::instance()->addMapLayer(map_layer, TRUE);
			map_layer_set.append(QgsMapCanvasLayer(map_layer));
			m_map_canvas->setExtent(map_layer->extent());
		}

		// if it is a raster layer
		else if (extention == ".tif" || extention == ".tiff" || extention == ".jpg" || extention == ".jpeg") {
		}
	}

	m_map_canvas->setLayerSet(map_layer_set);
}

void MapStreamView::update() {
	lin_algebra::vec2f pos = m_pos_stream->get_data();
	QgsPoint p(pos[0], pos[1]);

	// go to the current plane pos on the map
	m_map_canvas->setExtent(QgsRectangle(p, p));
	m_map_canvas->refresh();

	// draw the line of the plane's way
	m_plane_track->addPoint(p);
	m_map_canvas->refresh();
}

} // namespace gs
