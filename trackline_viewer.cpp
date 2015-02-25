#include "trackline_viewer.h"

#include <cmath>
#include <map>
#include <QtGui>
#include <float.h>


TracklineViewer::TracklineViewer(QWidget *parent) :
    QWidget(parent)
{
//    ptOrigin.setX(0.0);
//    ptOrigin.setY(height());
//    setGeometry(0,0,800,600);
    setStyleSheet("background-color:white;");

    ptMaxD.setX(DBL_MIN);
    ptMaxD.setY(DBL_MIN);
    ptMinD.setX( DBL_MAX );
    ptMinD.setY( DBL_MAX );
    scale = 0;
    d_b_xy_inverted = false;
}

TracklineViewer::~TracklineViewer()
{
}

double TracklineViewer::xWtoS(double x)
{
    return 0.0 + (x - ptMin.x()) * scale;
}

double TracklineViewer::yWtoS(double y)
{
    return height() - (y - ptMin.y()) * scale;
}

double TracklineViewer::xStoW(double x)
{
    return (x - 0.0) / scale + ptMin.x();
}

double TracklineViewer::yStoW(double y)
{
    return -(y - height()) / scale + ptMin.y();
}

QPointF TracklineViewer::WorldToScreen(const QPointF& pt)
{
    return QPointF( xWtoS(pt.x()), yWtoS(pt.y()) );
}

QPointF TracklineViewer::ScreenToWorld(const QPointF &pt)
{
    return QPointF( xStoW(pt.x()), yStoW(pt.y()) );
}

//void PlaneViewer::AddBeamStripe(MBSwath *pbs)
//{
//    vecpBS.push_back(pbs);
//}

void TracklineViewer::drawGrid(QPainter *painter)
{
    double ax, bx, ay, by;
    int ix, iy;
    adjustAxis((double)ptMin.x(), (double)ptMax.x(), ax, bx, ix);
    adjustAxis((double)ptMin.y(), (double)ptMax.y(), ay, by, iy);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(Qt::gray, 1, Qt::DashDotLine, Qt::RoundCap));

    double intervalX = (bx - ax) / ix;
    double intervalY = (by - ay) / iy;
    for (int i = 0; ; i++)
    {
        double xLabel = ax + i * intervalX;
        double xScr = WorldToScreen(QPointF(xLabel, 0)).x();

//        double yLabel = ay + i * intervalY;
//        double yScr = WorldToScreen(QPointF(yLabel, 0)).y();

        if (xScr > width()) break;

        if (d_b_xy_inverted) {
            painter->setPen(QPen(Qt::gray, 1, Qt::DashDotLine, Qt::RoundCap));
            painter->drawLine(QPointF(xScr, 0), QPointF(xScr, height()));
            painter->setPen(QPen(Qt::black));
            painter->drawText(xScr - 50, 0, 100, 20, Qt::AlignHCenter | Qt::AlignTop, QString::number(xLabel));
        }
        else
        {
            painter->setPen(QPen(Qt::gray, 1, Qt::DashDotLine, Qt::RoundCap));
            painter->drawLine(QPointF(xScr, 0), QPointF(xScr, height()));
            painter->setPen(QPen(Qt::red));
            painter->drawText(xScr - 50, 0, 100, 20, Qt::AlignHCenter | Qt::AlignTop, QString::number(xLabel));
        }
    }

    for (int i = 0; ; i++)
    {
        double yLabel = ay + i * intervalY;
        double yScr = WorldToScreen(QPointF(0, yLabel)).y();

        if (yScr < 0) break;

        if (d_b_xy_inverted) {
            painter->setPen(QPen(Qt::gray, 1, Qt::DashDotLine, Qt::RoundCap));
            painter->drawLine(QPointF(0, yScr), QPointF(width(), yScr));
            painter->setPen(QPen(Qt::black));
            painter->drawText(0, yScr, 100, 20, Qt::AlignHCenter | Qt::AlignTop, QString::number(yLabel));
        }
        else
        {
            painter->setPen(QPen(Qt::gray, 1, Qt::DashDotLine, Qt::RoundCap));
            painter->drawLine(QPointF(0, yScr), QPointF(width(), yScr));
            painter->setPen(QPen(Qt::red));
            painter->drawText(0, yScr, QString::number(yLabel));
        }
    }
}


void TracklineViewer::drawTracklines(QPainter *painter)
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
    };

    for (int i = 0; i < d_tracklines.size(); i++)
    {
        for ( int j = 0; j < d_tracklines[i].size() - 1; j++ )
        {
            painter->setPen(colorForIds[uint(i) % 6]);
            painter->drawLine(WorldToScreen(d_tracklines[i][j]), WorldToScreen(d_tracklines[i][j + 1]));
        }
    }
}

void TracklineViewer::addPoint(QPointF pt, QPainter *painter)
{
    painter->drawEllipse(WorldToScreen(pt), 3, 3);
}

void TracklineViewer::addTrackLine(DataRow pts, QPointF pt_min, QPointF pt_max)
{
    if (pt_min.x() == 0 && pt_max.x() == 0) {
        float d_lon_min = pts[0].x;
        float d_lon_max = pts[0].x;
        float d_lat_min = pts[0].y;
        float d_lat_max = pts[0].y;
        size_t n_pos = pts.size();
        for (size_t i = 0; i < n_pos; i++) {
            d_lon_min = d_lon_min < pts[i].x ? d_lon_min : pts[i].x;
            d_lon_max = d_lon_max > pts[i].x ? d_lon_max : pts[i].x;
            d_lat_min = d_lat_min < pts[i].y ? d_lat_min : pts[i].y;
            d_lat_max = d_lat_max > pts[i].y ? d_lat_max : pts[i].y;
        };//! recaculate
//        ptMaxD = QPointF(d_lon_max, d_lat_max);
//        ptMinD = QPointF(d_lon_min, d_lat_min);
////        ptMaxD = QPointF(d_lat_max, d_lon_max);
////        ptMinD = QPointF(d_lat_min, d_lon_min);
        ptMaxD.setX( d_lon_max );
        ptMaxD.setY( d_lat_max );
        ptMinD.setX( d_lon_min );
        ptMinD.setY( d_lat_min );
    }
    else {
        ptMaxD.setX( std::max( pt_max.x(), ptMaxD.x() ) );
        ptMaxD.setY( std::max( pt_max.y(), ptMaxD.y() ) );
        ptMinD.setX( std::min( pt_min.x(), ptMinD.x() ) );
        ptMinD.setY( std::min( pt_min.y(), ptMinD.y() ) );
    }

    TrackLine _line(pts.size());
    for (size_t i = 0; i < pts.size(); i++) {
        _line[i] = QPointF(pts[i].x, pts[i].y);
    }
    d_tracklines.push_back(_line);


    centerView();
    refreshPixmap();
}


void TracklineViewer::refreshPixmap()
{
    pixmap = QPixmap(size());
    pixmap.fill(this, 0, 0);

    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawGrid(&painter);
    drawTracklines(&painter);
    update();
}

void TracklineViewer::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
}

void TracklineViewer::resizeEvent(QResizeEvent */*event*/)
{
//    if ( !vecpBS.empty() )
//    {
////        centerView();
//        if (scale)
//            scale *= std::max((double)width() / lastRect.width(), (double)height() / lastRect.height());    // Incorrect
//        refreshPixmap();
//        update();
//    }
    lastRect = rect();
}

void TracklineViewer::centerView()
{
    scale = std::min(height() / (ptMaxD.y() - ptMinD.y()), width() / (ptMaxD.x() - ptMinD.x())) * 0.95;

    ptMin = ptMinD;
    ptMax = ptMaxD;

    //! make beamstripe center
//    ptOrigin.setX(0.0);
//    ptOrigin.setY(height());

    QPointF point_screen_center( width() / 2, height() / 2 );
    QPointF point_world_center( (ptMaxD.x() + ptMinD.x()) / 2 , (ptMaxD.y() + ptMinD.y()) / 2 );

    QPointF ptT = (ScreenToWorld(point_screen_center) - point_world_center);
//    ptOrigin += ptT;
    ptMin -= ptT;
    ptMax += ptT;
}

void TracklineViewer::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void TracklineViewer::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->buttons() & Qt::MidButton )
    {
        ptMin -= ScreenToWorld(event->pos()) - ScreenToWorld(lastPos);
        ptMax -= ScreenToWorld(event->pos()) - ScreenToWorld(lastPos);
        refreshPixmap();
        update();
    }
    lastPos = event->pos();
}

void TracklineViewer::wheelEvent(QWheelEvent *event)
{
    if ((ptMaxD.x() - ptMinD.x()) * scale < 100) {
        QMessageBox::information(this, tr(""),tr("cannot zoom out any more"));
        QPointF ptMW = ScreenToWorld(event->pos()); // Mouse point coor in the word
        scale *= 4 / 3.0;
        ptMin = ptMW - (ptMW - ptMin) / (4 / 3.0);
        ptMax = ptMW - (ptMW - ptMax) / (4 / 3.0);
        refreshPixmap();
        update();
    }
    else if((ptMaxD.x() - ptMinD.x()) * scale > 100000) {
        QMessageBox::information(this, tr(""),tr("cannot zoom in any more"));
        QPointF ptMW = ScreenToWorld(event->pos()); // Mouse point coor in the word
        scale *= 0.75;
        ptMin = ptMW - (ptMW - ptMin) / 0.75;
        ptMax = ptMW - (ptMW - ptMax) / 0.75;
        refreshPixmap();
        update();
    }
    else
    {
        QPointF ptMW = ScreenToWorld(event->pos()); // Mouse point coor in the word
        float change = pow(0.75, -event->delta() / 120);
        scale *= change;
        ptMin = ptMW - (ptMW - ptMin) / change;
        ptMax = ptMW - (ptMW - ptMax) / change;
        refreshPixmap();
        update();
    }
}

