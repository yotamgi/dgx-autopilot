#include "map_stream_view.h"
#include <QtGui>

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(new QPushButton("&A"));
	layout->addWidget(new QPushButton("&B"));

	layout->addWidget(new gs::MapStreamView("./data/map", "ogr"));

	QWidget* window = new QWidget();
	window->setLayout(layout);
	window->show();

	return app.exec();
}
