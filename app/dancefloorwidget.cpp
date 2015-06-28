#include "dancefloorwidget.h"
#include "utils.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QTimerEvent>
#include <math.h>
#include "GuiSettings.h"
#include "RegionNode.h"

Dancefloorwidget::Dancefloorwidget(QWidget *parent) :
    QWidget(parent),
    m_regionQuery(nullptr),
    m_regionEdit(nullptr),
    m_rubberBand(nullptr),
    m_dragged(false)
{
    m_cellsize = guisettings->df_cellsize;
    m_cellspace = guisettings->df_cellspace;

    setWindowTitle(tr("dLite floor"));
//    setWindowFlags(Qt::Tool | Qt::WindowTitleHint  |
    setWindowFlags(Qt::WindowTitleHint  |
                   Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
}

Dancefloorwidget::~Dancefloorwidget() { }

void Dancefloorwidget::setModel(Dancefloor *model)
{
    m_dfModel = model;
    m_xsize = model->getXsize();
    m_ysize = model->getYsize();
    setFixedWidth(m_xsize*(m_cellsize+m_cellspace));
    setFixedHeight(m_ysize*(m_cellsize+m_cellspace));

    // TODO restore from saved & allowed saved layouts
    // also, snapping...
    move(1100, 10);
}

bool Dancefloorwidget::cellHasLights(int x, int y) {
    return m_dfModel->hasPixel(x,y);
}

// -----------------------------------------------------------------------------
// paintEvent
void Dancefloorwidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), guisettings->df_bgColor);

    // Draw the outline
    QPen cellPen(guisettings->df_cellSepColor);
    painter.setPen(cellPen);
    painter.setFont(guisettings->sg_HzFont);

    QRect cell(0,0, m_cellsize, m_cellsize);
    for (int y=0; y<m_ysize; ++y) {
        for (int x=0; x<m_xsize; ++x) {
            cell.moveLeft(m_cellspace/2 + x*(m_cellsize+m_cellspace));
            cell.moveTop(m_cellspace/2 + y*(m_cellsize+m_cellspace));

            if (cellHasLights(x,y)) {
                if (m_regionQuery)
                    painter.fillRect(cell, m_regionQuery(QPoint(x,y)));
                else
                    painter.fillRect(cell, m_dfModel->getQColor(x,y));
            }
            else
                painter.fillRect(cell, guisettings->df_noCellColor);
            painter.drawRect(cell);

            if (y==0) {
                painter.save();
                painter.setPen(guisettings->sg_textColor);
                painter.drawText(cell, Qt::AlignTop|Qt::AlignLeft, QString::number(x));
                painter.restore();
            }

            if (x==0) {
                painter.save();
                painter.setPen(guisettings->sg_textColor);
                painter.drawText(cell, Qt::AlignTop|Qt::AlignLeft, QString::number(y));
                painter.restore();
            }

            // GROSS HACK!! Hardwired bars showing where the plywood is
            // Vertical bars
            int xstart= (y<9) ? 4 : 6;
            int xend= (y<9) ? 19 : 17;
            if ((x>xstart) && (x<xend) && (y>2) && (y<17) && (x%2==1)) {
                    QRect wood = cell;
                    wood.setLeft(cell.right()+2);
                    wood.setRight(cell.right()+m_cellspace);
                    wood.setTop(cell.top()-m_cellspace+2);
                    wood.setBottom(cell.bottom()+m_cellspace-1);
                    painter.fillRect(wood,guisettings->df_woodColor);
            }
            // Horizontal bars
            xstart= (y<10) ? 5 : 7;
            xend= (y<10) ? 18 : 16;
            if ((x>xstart) && (x<xend) && (y>2) && (y<18) && (y%2==1)) {
                    QRect wood = cell;
                    wood.setLeft(cell.left()-m_cellspace+1);
                    wood.setRight(cell.right()+m_cellspace);
                    wood.setTop(cell.top()-m_cellspace+1);
                    wood.setBottom(cell.top()-1);
                    painter.fillRect(wood,guisettings->df_woodColor);
            }
        }
    }
}

#ifndef INLINE
int Dancefloorwidget::_getIndex(int x, int y)
{
    Q_ASSERT(x >= 0 && x < m_xsize && y >= 0 && y < m_ysize);
    // XXX could clamp
    return m_xsize*y + x;
}
#endif

// -----------------------------------------------------------------------------
// Region editing
void Dancefloorwidget::setRegionEdit()
{
    if (m_selectedRegionNodes.size() == 1) {
        QSet<RegionNode*>::const_iterator i = m_selectedRegionNodes.constBegin();
        m_regionQuery = (*i)->getQueryDelegate();
        m_regionEdit  = (*i)->getEditDelegate();
    }
    else {
        m_regionQuery = nullptr;
        m_regionEdit  = nullptr;
    }
}

void Dancefloorwidget::regionSelected(RegionNode *chosen)
{
    m_selectedRegionNodes.insert(chosen);
    setRegionEdit();
    update();  // XXX Can optimize
}

void Dancefloorwidget::regionDeselected(RegionNode *chosen)
{
    m_selectedRegionNodes.remove(chosen);
    setRegionEdit();
    update();  // XXX Can optimize
}

bool Dancefloorwidget::editingRegion()
{
    return (m_regionEdit && m_regionQuery);
}

// Find which cell was clicked, given a point in the widget
QPoint Dancefloorwidget::findCell(QPoint p)
{
    int xCell = p.x()/(m_cellsize+m_cellspace);
    int yCell = p.y()/(m_cellsize+m_cellspace);
    return QPoint(xCell,yCell);
}

// Find the point in the widget, given the cell that was clicked.
QPoint Dancefloorwidget::findPos(QPoint cell)
{
    int x = cell.x()*(m_cellsize+m_cellspace);
    int y = cell.y()*(m_cellsize+m_cellspace);
    return QPoint(x,y);
}

// -----------------------------------------------------------------------------
// Mouse Events
void Dancefloorwidget::mousePressEvent(QMouseEvent *event)
{
    if (! editingRegion())
        return;

    m_dragStart = event->pos();
    QPoint cell = findCell(event->pos());
    m_dragCellStart = cell;

    m_dragged = false;
}

void Dancefloorwidget::mouseMoveEvent(QMouseEvent *event)
{
    if (! editingRegion())
        return;

    // Minimum distance for "drag"
    QPoint diff = event->pos() - m_dragStart;
    if (diff.manhattanLength() < 5)
        return;

    m_dragged = true;
    if (!m_rubberBand)
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    m_rubberBand->setGeometry(QRect(m_dragStart, event->pos()).normalized());
    m_rubberBand->show();

    QPoint currentCell = findCell(event->pos());

    // Fill all cells between drag start cell & current cell
    int xstart = qMin(m_dragCellStart.x(), currentCell.x());
    int xend   = qMax(m_dragCellStart.x(), currentCell.x());
    int ystart = qMin(m_dragCellStart.y(), currentCell.y());
    int yend   = qMax(m_dragCellStart.y(), currentCell.y());
    for (int y=ystart; y<=yend; ++y)
        for (int x=xstart; x<=xend; ++x) {
            QPoint cell = QPoint(x,y);
            if (event->modifiers() == Qt::ControlModifier)
                m_regionEdit(cell, false);
            else
                m_regionEdit(cell, true);
        }

    int right = xend+1;
    int bottom = yend+1;
    QPoint topLeft     = findPos(QPoint(xstart, ystart));
    QPoint bottomRight = findPos(QPoint(right, bottom ));
    QRect updateRect( topLeft, bottomRight);
    update(updateRect);
}

void Dancefloorwidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (! editingRegion())
        return;

    // If we didn't drag, treat as a click
    if (! m_dragged) {
        if (event->modifiers() == Qt::ControlModifier)
            m_regionEdit(m_dragCellStart, false);
        else
            m_regionEdit(m_dragCellStart, true);
    }
    // reset for next time
    m_dragged = false;

    // Cleanup & redraw.
    if (m_rubberBand)
        m_rubberBand->hide();
    update();
}

// -----------------------------------------------------------------------------
// Public slots
void Dancefloorwidget::showAndRaise()
{
    show();
    raise();
}
