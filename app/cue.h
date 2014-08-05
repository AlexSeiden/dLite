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
    //ColorPalette *palette;
    int     xoffset, yoffset;
    float   scale;
    bool    _active;
    Dancefloormodel *_model;
};

#endif // CUE_H
