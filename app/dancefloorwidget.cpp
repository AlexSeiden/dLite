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
    _regionQuery(nullptr),
    _regionEdit(nullptr),
    _rubberBand(nullptr),
    _dragged(false)
{
    _cellsize = GuiSettings::df_cellsize;
    _cellspace = GuiSettings::df_cellspace;

    setWindowTitle(tr("dLite floor"));
//    setWindowFlags(Qt::Tool | Qt::WindowTitleHint  |
    setWindowFlags(Qt::WindowTitleHint  |
                   Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
}

Dancefloorwidget::~Dancefloorwidget() { }

void Dancefloorwidget::setModel(Dancefloor *model)
{
    _dfModel = model;
    _xsize = model->getXsize();
    _ysize = model->getYsize();
    setFixedWidth(_xsize*(_cellsize+_cellspace));
    setFixedHeight(_ysize*(_cellsize+_cellspace));

    // TODO restore from saved & allowed saved layouts
    // also move to right side of screen?
    // also, snapping...
    move(1100, 10);
}

bool Dancefloorwidget::cellHasLights(int x, int y) {
    return _dfModel->hasPixel(x,y);
}

// -----------------------------------------------------------------------------
// paintEvent
void Dancefloorwidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), GuiSettings::df_bgColor);

    // Draw the outline
    QPen cellPen(GuiSettings::df_cellSepColor);
    painter.setPen(cellPen);

    QRect cell(0,0, _cellsize, _cellsize);
    for (int y=0; y<_ysize; ++y) {
        for (int x=0; x<_xsize; ++x) {
            cell.moveLeft(_cellspace/2 + x*(_cellsize+_cellspace));
            cell.moveTop(_cellspace/2 + y*(_cellsize+_cellspace));

            if (cellHasLights(x,y)) {
                if (_regionQuery)
                    painter.fillRect(cell, _regionQuery(QPoint(x,y)));
                else
                    painter.fillRect(cell, _dfModel->getQColor(x,y));
            }
            else
                painter.fillRect(cell, GuiSettings::df_noCellColor);
            painter.drawRect(cell);

            // GROSS HACK!! Hardwired bars
            // Vertical bars
            int xstart= (y<9) ? 4 : 6;
            int xend= (y<9) ? 19 : 17;
            if ((x>xstart) && (x<xend) && (y>2) && (y<17) && (x%2==1)) {
                    QRect wood = cell;
                    wood.setLeft(cell.right()+2);
                    wood.setRight(cell.right()+_cellspace);
                    wood.setTop(cell.top()-_cellspace+2);
                    wood.setBottom(cell.bottom()+_cellspace-1);
                    painter.fillRect(wood,GuiSettings::df_woodColor);
            }
            // Horizontal bars
            xstart= (y<10) ? 5 : 7;
            xend= (y<10) ? 18 : 16;
            if ((x>xstart) && (x<xend) && (y>2) && (y<18) && (y%2==1)) {
                    QRect wood = cell;
                    wood.setLeft(cell.left()-_cellspace+1);
                    wood.setRight(cell.right()+_cellspace);
                    wood.setTop(cell.top()-_cellspace+1);
                    wood.setBottom(cell.top()-1);
                    painter.fillRect(wood,GuiSettings::df_woodColor);
            }
        }
    }
}

#ifndef INLINE
int Dancefloorwidget::_getIndex(int x, int y)
{
    Q_ASSERT(x >= 0 && x < _xsize && y >= 0 && y < _ysize);
    // XXX could clamp
    return _xsize*y + x;
}
#endif

// -----------------------------------------------------------------------------
// Region editing
void Dancefloorwidget::setRegionEdit()
{
    if (_selectedRegionNodes.size() == 1) {
        QSet<RegionNode*>::const_iterator i = _selectedRegionNodes.constBegin();
        _regionQuery = (*i)->getQueryDelegate();
        _regionEdit  = (*i)->getEditDelegate();
    }
    else {
        _regionQuery = nullptr;
        _regionEdit  = nullptr;
    }
}

void Dancefloorwidget::regionSelected(RegionNode *chosen)
{
    _selectedRegionNodes.insert(chosen);
    setRegionEdit();
    update();  // XXX Can optimize
}

void Dancefloorwidget::regionDeselected(RegionNode *chosen)
{
    _selectedRegionNodes.remove(chosen);
    setRegionEdit();
    update();  // XXX Can optimize
}

bool Dancefloorwidget::editingRegion()
{
    return (_regionEdit && _regionQuery);
}

// Find which cell was clicked, given a point in the widget
QPoint Dancefloorwidget::findCell(QPoint p)
{
    int xCell = p.x()/(_cellsize+_cellspace);
    int yCell = p.y()/(_cellsize+_cellspace);
    return QPoint(xCell,yCell);
}

// Find the point in the widget, given the cell that was clicked.
QPoint Dancefloorwidget::findPos(QPoint cell)
{
    int x = cell.x()*(_cellsize+_cellspace);
    int y = cell.y()*(_cellsize+_cellspace);
    return QPoint(x,y);
}

void Dancefloorwidget::mousePressEvent(QMouseEvent *event)
{
    if (! editingRegion())
        return;

    _dragStart = event->pos();
    QPoint cell = findCell(event->pos());
    _dragCellStart = cell;

    _dragged = false;
}

void Dancefloorwidget::mouseMoveEvent(QMouseEvent *event)
{
    if (! editingRegion())
        return;

    // Minimum distance for "drag"
    QPoint diff = event->pos() - _dragStart;
    if (diff.manhattanLength() < 5)
        return;

    _dragged = true;
    if (!_rubberBand)
        _rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    _rubberBand->setGeometry(QRect(_dragStart, event->pos()).normalized());
    _rubberBand->show();

    QPoint currentCell = findCell(event->pos());

    // Fill all cells between drag start cell & current cell
    int xstart = qMin(_dragCellStart.x(), currentCell.x());
    int xend   = qMax(_dragCellStart.x(), currentCell.x());
    int ystart = qMin(_dragCellStart.y(), currentCell.y());
    int yend   = qMax(_dragCellStart.y(), currentCell.y());
    for (int y=ystart; y<=yend; ++y)
        for (int x=xstart; x<=xend; ++x) {
            QPoint cell = QPoint(x,y);
            if (event->modifiers() == Qt::ControlModifier)
                _regionEdit(cell, false);
            else
                _regionEdit(cell, true);
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
    if (! _dragged) {
        if (event->modifiers() == Qt::ControlModifier)
            _regionEdit(_dragCellStart, false);
        else
            _regionEdit(_dragCellStart, true);
    }
    // reset for next time
    _dragged = false;

    // Cleanup & redraw.
    if (_rubberBand)
        _rubberBand->hide();
    update();
}
