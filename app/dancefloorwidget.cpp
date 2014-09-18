#include "dancefloorwidget.h"
#include "utils.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QTimerEvent>
#include <math.h>
#include "GuiSettings.h"

Dancefloorwidget::Dancefloorwidget(QWidget *parent) :
    QWidget(parent)
{
    cellsize = GuiSettings::df_cellsize;
    cellspace = GuiSettings::df_cellspace;

    setWindowTitle(tr("dLite floor"));
//    setWindowFlags(Qt::Tool | Qt::WindowTitleHint  |
    setWindowFlags(Qt::WindowTitleHint  |
                   Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
}

Dancefloorwidget::~Dancefloorwidget() { }

void Dancefloorwidget::setModel(Dancefloor *model)
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

//    QColor woodcol(186,176,153);
    QColor woodcol(130,123,112);

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

            // HACK!!
            // Vertical bars
            int xstart= (y<9) ? 4 : 6;
            int xend= (y<9) ? 19 : 17;
            if ((x>xstart) && (x<xend) && (y>2) && (y<17) && (x%2==1)) {
                    QRect wood = cell;
                    wood.setLeft(cell.right()+2);
                    wood.setRight(cell.right()+cellspace);
                    wood.setTop(cell.top()-cellspace+2);
                    wood.setBottom(cell.bottom()+cellspace-1);
                    painter.fillRect(wood,woodcol);
            }
            // Horizontal bars
            xstart= (y<10) ? 5 : 7;
            xend= (y<10) ? 18 : 16;
            if ((x>xstart) && (x<xend) && (y>2) && (y<18) && (y%2==1)) {
                    QRect wood = cell;
                    wood.setLeft(cell.left()-cellspace+1);
                    wood.setRight(cell.right()+cellspace);
                    wood.setTop(cell.top()-cellspace+1);
                    wood.setBottom(cell.top()-1);
                    painter.fillRect(wood,woodcol);
            }
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
