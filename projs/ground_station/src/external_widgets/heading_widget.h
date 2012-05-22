// Based on QFlightInstruments,
// Thanks to Marek Cel.

#ifndef _MSCHSI_H_
#define _MSCHSI_H_

#include <QtGui/QGraphicsView>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgGenerator>

class HeadingWidget : public QGraphicsView {
public:
    enum RendererType { Native, OpenGL, Image };

    HeadingWidget( QWidget *parent = 0 );
    ~HeadingWidget();

	void	init();

    void    set( double _psi );

private:

    void    clean( void );
    void    update_view( void );

    double  cur_Psi;    ///< [rad] current heading
    double  prv_Psi;    ///< [rad] previous heading

    const double pi;
    const double deg2rad;
    const double rad2deg;

    RendererType m_renderer;

    QGraphicsSvgItem *item_bg;
    QGraphicsSvgItem *item_casing;
    QGraphicsSvgItem *item_face;
    QGraphicsSvgItem *item_markers;
    QGraphicsSvgItem *item_shadow;
};

#endif // _MSCHSI_H_
