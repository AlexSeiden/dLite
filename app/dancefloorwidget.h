#ifndef DANCEFLOORWIDGET_H
#define DANCEFLOORWIDGET_H

#include <QWidget>
#include <QSet>
#include "lightcolor.h"
#include "DanceFloor.h"
#include <QRubberBand>
#include <functional>
#include <QPoint>

class RegionNode;
typedef  std::function<QColor(QPoint)> PixelQueryDelegate_t;
typedef  std::function<void (QPoint, bool)> PixelEditDelegate_t;

class Dancefloorwidget : public QWidget
{
    Q_OBJECT

public:
    explicit Dancefloorwidget(QWidget *parent = 0);
    ~Dancefloorwidget();

    void setModel(Dancefloor *model);
    void setHasLights(int x, int y, bool status);

    void paintEvent(QPaintEvent *event);
    // We'll need these when implementing path & region selection
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setRegionEdit();

public slots:
    void regionSelected(RegionNode *chosen);
    void regionDeselected(RegionNode *chosen);
    void showAndRaise();

private:
    bool    cellHasLights(int x, int y);
    bool    editingRegion();
    QPoint  findCell(QPoint p);
    QPoint  findPos(QPoint cell);

    Dancefloor* _dfModel;
    int         _xsize;
    int         _ysize;
    int         _cellsize;
    int         _cellspace;

    QSet<RegionNode*>       _selectedRegionNodes;
    PixelQueryDelegate_t    _regionQuery;
    PixelEditDelegate_t     _regionEdit;
    QRubberBand*		    _rubberBand;
    QPoint                  _dragStart;
    QPoint                  _dragCellStart;
    bool                    _dragged;

#ifdef INLINE
    int _getIndex(int x, int y) {return xsize*y + x;}
#else
    // This one has range checking
    int _getIndex(int x, int y);
#endif

};

#endif // DANCEFLOORWIDGET_H
