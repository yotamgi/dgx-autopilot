#include "heading_widget.h"

#include <QtSvg/QSvgRenderer>
#include <QtSvg/QSvgGenerator>

#include <math.h>
#include <iostream>

HeadingWidget::HeadingWidget( QWidget *parent ) :
    QGraphicsView( parent ),
    pi( 4*atan(1.0) ),
    deg2rad( pi/180 ),
    rad2deg( 180/pi ),
    m_renderer(Native),
    item_bg     ( 0 ),
    item_casing ( 0 ),
    item_face   ( 0 ),
    item_markers( 0 ),
    item_shadow ( 0 )
{
    cur_Psi = 0.0;
    prv_Psi = 0.0;

    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::HighQualityAntialiasing, true);
}

HeadingWidget::~HeadingWidget( void ) {
    clean();
}

void HeadingWidget::init( void ) {
    clean();

    item_bg      = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/any_bg.svg");
    item_casing  = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/any_casing.svg");
    item_face    = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/hsi_face.svg");
    item_markers = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/hsi_markers.svg");
    item_shadow  = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/any_shadow.svg");

    double x = 325;
    double y = 325;

    item_bg->setFlags(QGraphicsItem::ItemClipsToShape);
    item_bg->setCacheMode(QGraphicsItem::NoCache);
    item_bg->scale( (double)width()/x, (double)height()/y );
    item_bg->setZValue(250);

    item_casing->setFlags(QGraphicsItem::ItemClipsToShape);
    item_casing->setCacheMode(QGraphicsItem::NoCache);
    item_casing->scale( (double)width()/x, (double)height()/y );
    item_casing->setZValue(300);

    item_face->setFlags(QGraphicsItem::ItemClipsToShape);
    item_face->setCacheMode(QGraphicsItem::NoCache);
    item_face->scale( (double)width()/x, (double)height()/y );
    item_face->setTransformOriginPoint( x/2, y/2 );
    item_face->setZValue(1);

    item_markers->setFlags(QGraphicsItem::ItemClipsToShape);
    item_markers->setCacheMode(QGraphicsItem::NoCache);
    item_markers->scale( (double)width()/x, (double)height()/y );
    item_markers->setZValue(50);

    item_shadow->setFlags(QGraphicsItem::ItemClipsToShape);
    item_shadow->setCacheMode(QGraphicsItem::NoCache);
    item_shadow->scale( (double)width()/x, (double)height()/y );
    item_shadow->setZValue(100);

    scene()->clear();

    scene()->addItem( item_bg );
    scene()->addItem( item_casing );
    scene()->addItem( item_face );
    scene()->addItem( item_markers );
    scene()->addItem( item_shadow );
}

void  HeadingWidget::set( double _psi ) {
    cur_Psi = _psi*deg2rad;
    update_view( );
}

void HeadingWidget::clean( void ) {
    if ( item_casing )  delete item_casing;  item_casing  = 0;
    if ( item_bg )      delete item_bg;      item_bg      = 0;
    if ( item_face )    delete item_face;    item_face    = 0;
    if ( item_markers ) delete item_markers; item_markers = 0;
    if ( item_shadow )  delete item_shadow;  item_shadow  = 0;
}

void HeadingWidget::update_view( void ) {

    if ( prv_Psi != cur_Psi ) {

        item_face->setRotation( rad2deg * prv_Psi );
        item_face->setRotation( -rad2deg * cur_Psi );

        prv_Psi = cur_Psi;

        scene()->update();

    }
}
