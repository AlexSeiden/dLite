#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "dancefloormodel.h"
#include <QObject>

class Cue : public QObject
{
    Q_OBJECT
public:
    explicit Cue(QObject *parent = 0, Dancefloormodel *dfmodel = 0);

signals:
    void lightChanged(int x, int y, Lightcolor lights);

public slots:
    void levelChanged(qreal level);

private:
    Dancefloormodel *_floor;
    bool    _active;

    int     xoffset, yoffset;
    float   scale;
#if 0
    // Food for thought
    ColorPalette *palette;
    int   counter;
    int   measure;
    int   beatWithinMeasure;
#endif
};

class BoxCue : public Cue
{
    Q_OBJECT

public:
    explicit BoxCue(QObject *parent = 0, Dancefloormodel *dfmodel = 0);
    

};




#endif // CUE_H
