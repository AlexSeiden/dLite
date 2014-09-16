#ifndef DANCEFLOORWIDGET_H
#define DANCEFLOORWIDGET_H

#include <QWidget>
#include "lightcolor.h"
#include "DanceFloor.h"

class Dancefloorwidget : public QWidget
{
    Q_OBJECT
public:
    explicit Dancefloorwidget(QWidget *parent = 0);
    ~Dancefloorwidget();

    void setModel(Dancefloor *model);
    void setHasLights(int x, int y, bool status);

public:
    void paintEvent(QPaintEvent *event);
    // We'll need these when implementing path & region selection
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void infoMessage(const QString &message, int intervalMs);

private:
    Dancefloor *dfModel;
    bool    cellHasLights(int x, int y);
    int     xsize;
    int     ysize;

    int     cellsize;
    int     cellspace;

#ifdef INLINE
    int _getIndex(int x, int y) {return xsize*y + x;}
#else
    // This one has range checking
    int _getIndex(int x, int y);
#endif

};

#endif // DANCEFLOORWIDGET_H
