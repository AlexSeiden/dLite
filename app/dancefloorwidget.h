#ifndef DANCEFLOORWIDGET_H
#define DANCEFLOORWIDGET_H

#include <QWidget>
#include "lightcolor.h"

class Dancefloorwidget : public QWidget
{
    Q_OBJECT
public:
    explicit Dancefloorwidget(QWidget *parent = 0);
    ~Dancefloorwidget();

    void setGrid(int xsize, int ysize);
    void setHasLights(int x, int y, bool status);
    void setLightcolor(int x, int y, Lightcolor lc);

public:
    // QWidget
    void paintEvent(QPaintEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void infoMessage(const QString &message, int intervalMs);

public slots:
    void reset();

private:
    bool    cellHasLights(int x, int y);
    int xsize;
    int ysize;
    bool *active;
    QColor *lightqcolors;


    int cellsize;
    int cellspace;
    QColor bgColor;
    QColor cellSepColor;
    QColor panelSepColor;
    QColor noCellColor;

#ifdef INLINE
    int _getIndex(int x, int y) {return xsize*y + x;}
#else
    int _getIndex(int x, int y);
#endif

};

#endif // DANCEFLOORWIDGET_H
