#include "cue.h"
#include "utils.h"
#include "lightcolor.h"
#include "dancefloormodel.h"

Cue::Cue(QObject *parent, Dancefloormodel *dfmodel) :
    QObject(parent),
    _floor(dfmodel),
    _active(true),
    xoffset(10),
    yoffset(7),
    scale(1.0)
{
    CHECKED_CONNECT(this, SIGNAL(lightChanged(int,int,Lightcolor)),
        _floor, SLOT(lightChanged(int,int,Lightcolor)));

}


void Cue::levelChanged(qreal level)
{
    if (!_active) return;

    Lightcolor lightc = Lightcolor::LightcolorF(level, level, level);
    emit(lightChanged(xoffset, yoffset, lightc));
}
