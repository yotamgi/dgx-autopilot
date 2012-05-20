#include "map_stream_view.h"
#include <qgis/qgsproviderregistry.h>
#include <qgis/qgsvectorlayer.h>
#include <qgis/qgsrasterlayer.h>
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
 	m_plane_track->setColor(QColor(255, 0, 0, 70));
 	m_plane_track->setWidth(4);

 	m_plane_curr_pos = new QgsVertexMarker(m_map_canvas);
 	m_plane_curr_pos->setIconType(QgsVertexMarker::ICON_CROSS);
 	m_plane_curr_pos->setColor(QColor(150, 0, 0, 255));
 	m_plane_curr_pos->setIconSize(20);
 	m_plane_curr_pos->setPenWidth(4);

	m_mouse_emit = new QgsMapToolEmitPoint(m_map_canvas);
	m_map_canvas->setMapTool(m_mouse_emit);

	m_clear_button = new QPushButton("&Clear Track");

	m_show_truck_checkbox = new QCheckBox("&Show Track");
	m_show_truck_checkbox->setChecked(true);

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(m_clear_button, 0, 0);
	layout->addWidget(m_show_truck_checkbox, 0, 1);
	layout->addWidget(m_map_canvas, 1, 0, 1, 2);
	setLayout(layout);

 	m_timer = new QTimer(this);
 	connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer->start(1000*m_update_time);

	m_mouse_emit->activate();
	connect(m_mouse_emit, SIGNAL(canvasClicked(const QgsPoint &,Qt::MouseButton)),
			this, SLOT(clicked(const QgsPoint &, Qt::MouseButton)));

	connect(m_clear_button, SIGNAL(clicked()),
			this, SLOT(clear_track()));

	connect(m_show_truck_checkbox, SIGNAL(stateChanged(int)),
			this, SLOT(show_track_cb_changed(int)));

}

MapStreamView::~MapStreamView() {
	delete m_map_canvas;
	delete m_plane_track;
	delete m_timer;
	delete m_mouse_emit;
	delete m_clear_button;
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
	m_dots.push_back(dot);
}

void MapStreamView::clear_track() {
	m_plane_track->reset();
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
			QgsVectorLayer* vec_layer = new QgsVectorLayer(tr(f->path().c_str()), tr(f->path().filename().c_str()), QString("ogr"));
			QgsSingleSymbolRenderer *map_renderer = new QgsSingleSymbolRenderer(vec_layer->geometryType());
			vec_layer->setRenderer(map_renderer);
			if (!vec_layer->isValid()) {
				throw std::runtime_error("Couldn't load the map layer");
			}
			// Add the Vector Layer to the Layer Registry
			QgsMapLayerRegistry::instance()->addMapLayer(vec_layer, TRUE);
			map_layer_set.append(QgsMapCanvasLayer(vec_layer));
			m_map_canvas->setExtent(vec_layer->extent());
		}

		// if it is a raster layer
		else if (extention == ".tif" || extention == ".tiff" || extention == ".jpg" || extention == ".jpeg") {
			std::cout << "added raster layer " << f->path().filename().c_str() << std::endl;
			QgsRasterLayer* raster_layer = new QgsRasterLayer(
					tr(f->path().c_str()),
					tr(f->path().filename().c_str())
			);
			if (!raster_layer->isValid()) {
				throw std::runtime_error("Couldn't load the raster layer");
			}

			QgsMapLayerRegistry::instance()->addMapLayer(raster_layer, TRUE);
			map_layer_set.append(QgsMapCanvasLayer(raster_layer));
			m_map_canvas->setExtent(raster_layer->extent());
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
	m_plane_curr_pos->setCenter(p);
	m_map_canvas->refresh();
}

void MapStreamView::show_track_cb_changed(int state) {
	if (m_show_truck_checkbox->isChecked()) {
		m_map_canvas->scene()->addItem(m_plane_track);
	} else {
		m_map_canvas->scene()->removeItem(m_plane_track);
	}
}

} // namespace gs
