#include "dancefloorwidget.h"
#include "utils.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QTimerEvent>
#include <math.h>
#include "GuiColors.h"

Dancefloorwidget::Dancefloorwidget(QWidget *parent) :
    QWidget(parent)
{
    cellsize = GuiSettings::df_cellsize;
    cellspace = GuiSettings::df_cellspace;

    setWindowTitle(tr("dLite floor"));
    setWindowFlags(Qt::Tool | Qt::WindowTitleHint  |
                   Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
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
    move(1100, 10);
}

bool Dancefloorwidget::cellHasLights(int x, int y) {
    return dfModel->hasPixel(x,y);
}

void Dancefloorwidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), GuiSettings::df_bgColor);

    // Draw the outline
    QPen cellPen(GuiSettings::df_cellSepColor);
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
                painter.fillRect(cell, dfModel->getQColor(x,y));
            else
                painter.fillRect(cell, GuiSettings::df_noCellColor);
            painter.drawRect(cell);
        }
    }
}

#ifndef INLINE
int Dancefloorwidget::_getIndex(int x, int y)
{
    Q_ASSERT(x >= 0 && x < xsize && y >= 0 && y < ysize);
    return xsize*y + x;
}
#endif
