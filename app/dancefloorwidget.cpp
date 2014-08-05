#include "dancefloorwidget.h"
#include "utils.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QTimerEvent>
#include <math.h>

Dancefloorwidget::Dancefloorwidget(QWidget *parent) :
    QWidget(parent)
{
    bgColor = QColor(30,30,80);
    panelSepColor = QColor(255,255,200);
    cellSepColor = QColor(100,100,100);
    noCellColor = QColor(60,60,60);
    cellsize = 20;
    cellspace = 4;

    timer = new QTimer(this);
    CHECKED_CONNECT(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);  // TODO make this updatable
}

Dancefloorwidget::~Dancefloorwidget() { }

void Dancefloorwidget::setModel(Dancefloormodel *model)
{
    dfModel = model;
    xsize = model->getXsize();
    ysize = model->getYsize();
    setFixedWidth(xsize*(cellsize+cellspace));
    setFixedHeight(ysize*(cellsize+cellspace));

    // TODO restore from saved & allowed saved layouts
    // also move to right side of screen?
    // also, snapping...
    move(800, 50);
}

bool Dancefloorwidget::cellHasLights(int x, int y) {
    return dfModel->hasPixel(x,y);
}

void Dancefloorwidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    //qDebug() << "DFW paint event";

    QPainter painter(this);
    painter.fillRect(rect(), bgColor);

    // Draw the outline
    QPen cellPen(cellSepColor);
    painter.setPen(cellPen);
//    painter.drawLine(rect().topLeft(), rect().topRight());
//    painter.drawLine(rect().topRight(), rect().bottomRight());
//    painter.drawLine(rect().bottomRight(), rect().bottomLeft());
//    painter.drawLine(rect().bottomLeft(), rect().topLeft());

    QRect cell(0,0, cellsize, cellsize);
    for (int y=0; y<ysize; ++y) {
        for (int x=0; x<xsize; ++x) {
            cell.moveLeft(cellspace/2 + x*(cellsize+cellspace));
            cell.moveTop(cellspace/2 + y*(cellsize+cellspace));
            // TODO shouldn't recalc index twice...
            if (cellHasLights(x,y))
                //painter.fillRect(cell, Qt::black);
                painter.fillRect(cell, cellGetColor(x,y));
            else
                painter.fillRect(cell, noCellColor);
            painter.drawRect(cell);
        }
    }
}

QColor Dancefloorwidget::cellGetColor(int x, int y)
{
    Lightcolor rgb = dfModel->getPixel(x,y);
    return QColor(rgb.getRed(),rgb.getBlue(),rgb.getGreen());
}

#ifndef INLINE
int Dancefloorwidget::_getIndex(int x, int y)
{
    Q_ASSERT(x >= 0 && x < xsize && y >= 0 && y < ysize);
    return xsize*y + x;
}
#endif
