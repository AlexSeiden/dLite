#include "dancefloorwidget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimerEvent>
#include <math.h>

Dancefloorwidget::Dancefloorwidget(QWidget *parent) :
    QWidget(parent), lightqcolors(NULL)
{
    bgColor = QColor(30,30,80);
    panelSepColor = QColor(255,255,200);
    cellSepColor = QColor(100,100,100);
    noCellColor = QColor(60,60,60);
    cellsize = 20;
    cellspace = 4;
}

Dancefloorwidget::~Dancefloorwidget()
{
#if 0
    if (lightqcolors) {
        delete lightqcolors;
        lightqcolors = NULL;
    }
#endif
}

void Dancefloorwidget::setModel(Dancefloormodel *model)
{
    dfModel = model;
    xsize = model->getXsize();
    ysize = model->getYsize();
    setFixedWidth(xsize*(cellsize+cellspace));
    setFixedHeight(ysize*(cellsize+cellspace));
}

bool Dancefloorwidget::cellHasLights(int x, int y) {
    return dfModel->hasPixel(x,y);
}

void Dancefloorwidget::reset()
{
}

void Dancefloorwidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

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
            if (cellHasLights(x,y))
                painter.fillRect(cell, Qt::black);
            else
                painter.fillRect(cell, noCellColor);
            painter.drawRect(cell);
        }
    }
}
void Dancefloorwidget::setLightcolor(int x, int y, Lightcolor lc)
{
    int index = _getIndex(x,y);
    lightqcolors[index].setRed(lc.getRed());
    lightqcolors[index].setGreen(lc.getGreen());
    lightqcolors[index].setBlue(lc.getBlue());
}

#ifndef INLINE
int Dancefloorwidget::_getIndex(int x, int y)
{
    Q_ASSERT(x >= 0 && x < xsize && y >= 0 && y < ysize);
    return xsize*y + x;
}
#endif