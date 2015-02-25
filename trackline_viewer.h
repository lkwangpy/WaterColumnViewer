#ifndef PLANEVIEWER_H
#define PLANEVIEWER_H

#include <QPixmap>
#include <QWidget>
#include <vector>
#include <cmath>

#include "types.h"

typedef QVector<QPointF> TrackLine;
/**
 * @brief The TacklineViewer class
 */
class TracklineViewer : public QWidget
{
    Q_OBJECT
public:
    explicit TracklineViewer(QWidget *parent = 0);
    ~TracklineViewer();
    
    QPointF ptOrigin;
    QPointF ptMin, ptMax;
    QPointF ptMinD, ptMaxD; // the draw object's min & max point
    double scale;
    QPoint lastPos;
    QRect lastRect;

    double xWtoS(double x);
    double yWtoS(double y);
    double xStoW(double x);
    double yStoW(double y);
    QPointF WorldToScreen(const QPointF &pt);
    QPointF ScreenToWorld(const QPointF &pt);
    void centerView();
    void drawGrid(QPainter *painter);
    void drawTracklines(QPainter *painter);
    void refreshPixmap();
    void addPoint(QPointF pt, QPainter *painter);
//    void addPoints(QVector<QPointF>, QPainter *painter);
    void addTrackLine(DataRow pts, QPointF pt_min = QPointF(), QPointF pt_max = QPointF());

    QPixmap pixmap;
    QVector<TrackLine> d_tracklines;

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    bool d_b_xy_inverted;
};


class PlaneViewerSetting
{
public:
    PlaneViewerSetting();

//    QPointF ptOrigin;
    QPointF ptMin, ptMax;
    double scale;
    QPoint ptCenter;

};

inline void adjustAxis(double min, double max, double &a, double &b, int &numTicks)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = std::pow(10.0, std::floor(std::log10(grossStep)));

    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }

    numTicks = int(std::ceil(max / step) - std::floor(min / step));
    if (numTicks < MinTicks)
        numTicks = MinTicks;
    a = std::floor(min / step) * step;
    b = std::ceil(max / step) * step;
}


#endif // PLANEVIEWER_H
