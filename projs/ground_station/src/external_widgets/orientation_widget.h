// Based on QFlightInstruments,
// Thanks to Marek Cel.

#ifndef _MSCADI_H_
#define _MSCADI_H_

#include <QtGui/QGraphicsView>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgGenerator>

class OrientationWidget : public QGraphicsView {
public:

	enum RendererType { Native, OpenGL, Image };


	OrientationWidget(QWidget *parent = 0);
	~OrientationWidget();

	void init();

	void set(double _roll, double _pitch);
	void setRoll(double _roll);
	void setPitch(double _pitch);

private:
    void clean();
    void update_view();

    double  cur_pitch;
    double  cur_roll;
    double  prv_pitch;
    double  prv_roll;

    const double pi;
    const double deg2rad;
    const double rad2deg;

    RendererType m_renderer;

    //QGraphicsPixmapItem *item_cas;
    QGraphicsSvgItem *item_cas;
    QGraphicsSvgItem *item_bg;
    QGraphicsSvgItem *item_shadow;
    QGraphicsSvgItem *item_roll;
    QGraphicsSvgItem *item_shd1;
    QGraphicsSvgItem *item_shd2;
    QGraphicsSvgItem *item_plt;
    QGraphicsSvgItem *item_spr;
    QGraphicsSvgItem *item_mkr;
    QGraphicsSvgItem *item_sky;
    QGraphicsSvgItem *item_grd;

};

#endif // _MSCADI_H_
