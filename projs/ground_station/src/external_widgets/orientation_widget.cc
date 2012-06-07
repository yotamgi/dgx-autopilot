#include "orientation_widget.h"

#include <QtSvg/QSvgRenderer>
#include <QtSvg/QSvgGenerator>

#include <math.h>
#include <iostream>

OrientationWidget::OrientationWidget(QWidget *parent):
    QGraphicsView( parent ),
    pi( 4*atan(1.0) ),
    deg2rad( pi/180 ),
    rad2deg( 180/pi ),
    m_renderer(Native),
    item_cas   ( 0 ),
    item_bg    ( 0 ),
    item_shadow( 0 ),
    item_roll  ( 0 ),
    item_shd1  ( 0 ),
    item_shd2  ( 0 ),
    item_plt   ( 0 ),
    item_spr   ( 0 ),
    item_mkr   ( 0 ),
    item_sky   ( 0 ),
    item_grd   ( 0 )
{
    cur_roll    = 0.0;
    cur_pitch   = 0.0;
    prv_roll    = 0.0;
    prv_pitch   = 0.0;

    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::HighQualityAntialiasing, true);
}

OrientationWidget::~OrientationWidget() {
    clean();
}

void OrientationWidget::init() {
    clean();

    item_cas    = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_casing.svg");
    item_bg     = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/any_bg.svg");
    item_shadow = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/any_shadow.svg");
    item_roll   = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_rollring.svg");
    item_shd1   = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_shadow1.svg");
    item_shd2   = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_shadow2.svg");
    item_plt    = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_face.svg");
    item_spr    = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_bg.svg");
    item_mkr    = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_markers.svg");
    item_sky    = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_sky.svg");
    item_grd    = new QGraphicsSvgItem(MEDIA_DIR "/widget_files/qfi/adi_ground.svg");

    double x = 330;
    double y = 330;

    item_cas->setFlags(QGraphicsItem::ItemClipsToShape);
    item_cas->setCacheMode(QGraphicsItem::NoCache);
    item_cas->scale( (double)width()/x, (double)height()/y );
    item_cas->setZValue(250);
    item_cas->setPos(-width()/2, -height()/2);

    item_bg->setFlags(QGraphicsItem::ItemClipsToShape);
    item_bg->setCacheMode(QGraphicsItem::NoCache);
    item_bg->scale( (double)width()/x, (double)height()/y );
    item_bg->setZValue(200);
    item_bg->setPos(-width()/2, -height()/2);

    item_shadow->setFlags(QGraphicsItem::ItemClipsToShape);
    item_shadow->setCacheMode(QGraphicsItem::NoCache);
    item_shadow->scale( (double)width()/x, (double)height()/y );
    item_shadow->setZValue(199);
    item_shadow->setPos(-width()/2, -height()/2);

    item_roll->setFlags(QGraphicsItem::ItemClipsToShape);
    item_roll->setCacheMode(QGraphicsItem::NoCache);
    item_roll->scale( (double)width()/x, (double)height()/y );
    item_roll->setTransformOriginPoint( x/2, y/2 );
    item_roll->setZValue(100);
    item_roll->setPos(-width()/2, -height()/2);

    item_shd1->setFlags(QGraphicsItem::ItemClipsToShape);
    item_shd1->setCacheMode(QGraphicsItem::NoCache);
    item_shd1->scale( (double)width()/x, (double)height()/y );
    item_shd1->setZValue(75);
    item_shd1->setPos(-width()/2, -height()/2);

    item_plt->setFlags(QGraphicsItem::ItemClipsToShape);
    item_plt->setCacheMode(QGraphicsItem::NoCache);
    item_plt->scale( (double)width()/x, (double)height()/y );
    item_plt->setTransformOriginPoint( x/2, y/2 );
    item_plt->setZValue(90);
    item_plt->setPos(-width()/2, -height()/2);

    item_shd2->setFlags(QGraphicsItem::ItemClipsToShape);
    item_shd2->setCacheMode(QGraphicsItem::NoCache);
    item_shd2->scale( (double)width()/x, (double)height()/y );
    item_shd2->setZValue(95);
    item_shd2->setPos(-width()/2, -height()/2);

    item_spr->setFlags(QGraphicsItem::ItemClipsToShape);
    item_spr->setCacheMode(QGraphicsItem::NoCache);
    item_spr->scale( (double)width()/x, (double)height()/y );
    item_spr->setTransformOriginPoint( x/2, y/2 );
    item_spr->setZValue(20);
    item_spr->setPos(-width()/2, -height()/2);

    item_mkr->setFlags(QGraphicsItem::ItemClipsToShape);
    item_mkr->setCacheMode(QGraphicsItem::NoCache);
    item_mkr->scale( (double)width()/x, (double)height()/y );
    item_mkr->setZValue(150);
    item_mkr->setPos(-width()/2, -height()/2);

    item_grd->setFlags(QGraphicsItem::ItemClipsToShape);
    item_grd->setCacheMode(QGraphicsItem::NoCache);
    item_grd->scale( (double)width()/x, (double)height()/y );
    item_grd->setZValue(1);
    item_mkr->setPos(-width()/2, -height()/2);

    item_sky->setFlags(QGraphicsItem::ItemClipsToShape);
    item_sky->setCacheMode(QGraphicsItem::NoCache);
    item_sky->scale( (double)width()/x, (double)height()/y );
    item_sky->setZValue(2);
    item_mkr->setPos(-width()/2, -height()/2);

    scene()->clear();

    scene()->addItem( item_cas );
    scene()->addItem( item_bg );
    scene()->addItem( item_shadow );
    scene()->addItem( item_roll );
    scene()->addItem( item_shd1 );
    scene()->addItem( item_shd2 );
    scene()->addItem( item_plt );
    scene()->addItem( item_spr );
    scene()->addItem( item_mkr );
    scene()->addItem( item_grd );
    scene()->addItem( item_sky );
}

void  OrientationWidget::set(double _roll, double _pitch) {
    cur_roll  = _roll;
    if ( _pitch > deg2rad * 22 )
        cur_pitch = deg2rad * 22 / 2.0;
    else if ( _pitch < deg2rad * -22 )
        cur_pitch = deg2rad * -22 / 2.0;
    else
        cur_pitch = _pitch / 2.0;
    update_view( );
}

void OrientationWidget::setRoll(double roll) {
	double _roll = deg2rad*roll;
    cur_roll = _roll;
    update_view( );
}

void OrientationWidget::setPitch(double pitch) {
	double _pitch = deg2rad*pitch;
    if ( _pitch > deg2rad * 22 )
        cur_pitch = deg2rad * 22 / 2.0;
    else if ( _pitch < deg2rad * -22 )
        cur_pitch = deg2rad * -22 / 2.0;
    else
        cur_pitch = _pitch / 2.0;
    update_view( );
}

void OrientationWidget::clean() {
    if ( item_cas )  delete item_cas;   item_cas  = 0;
    if ( item_bg )   delete item_bg;    item_bg   = 0;
    if ( item_shadow ) delete item_shadow; item_shadow = 0;
    if ( item_roll ) delete item_roll;  item_roll = 0;
    if ( item_shd1 ) delete item_shd1;  item_shd1 = 0;
    if ( item_shd2 ) delete item_shd2;  item_shd2 = 0;
    if ( item_plt )  delete item_plt;   item_plt  = 0;
    if ( item_spr )  delete item_spr;   item_spr  = 0;
    if ( item_mkr )  delete item_mkr;   item_mkr  = 0;
    if ( item_sky )  delete item_sky;   item_sky  = 0;
    if ( item_grd )  delete item_grd;   item_grd  = 0;
}

void OrientationWidget::update_view() {

    double r = 165;
    //double y = 160;
    //double x = 160;

    if ( prv_roll != cur_roll || prv_pitch != cur_pitch ) {

        double d  = r*sin( -prv_pitch );
        double dx = d*sin( prv_roll );
        double dy = d*cos( prv_roll );

        item_plt->moveBy( dx, dy );
        item_spr->moveBy( dx, dy );

        item_roll->setRotation( rad2deg * prv_roll );
        item_plt->setRotation( rad2deg * prv_roll );
        item_spr->setRotation( rad2deg * prv_roll );

        prv_roll  = cur_roll;
        prv_pitch = cur_pitch;

        item_roll->setRotation( -rad2deg * cur_roll );
        item_plt->setRotation( -rad2deg * cur_roll );
        item_spr->setRotation( -rad2deg * cur_roll );

        d  = r*sin( -cur_pitch );
        dx = d*sin( cur_roll );
        dy = d*cos( cur_roll );

        item_plt->moveBy( -dx, -dy );
        item_spr->moveBy( -dx, -dy );

        if ( cur_pitch > 0 ) {
            item_sky->setZValue(2);
            item_grd->setZValue(1);
        } else {
            item_sky->setZValue(1);
            item_grd->setZValue(2);
        }

        scene()->update();

    }

}

