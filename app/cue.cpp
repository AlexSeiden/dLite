#include "cue.h"
#include "utils.h"
#include "lightcolor.h"
#include "dancefloormodel.h"

Cue::Cue(QObject *parent, Dancefloormodel *dfmodel) :
    QObject(parent),
    xoffset(10),
    yoffset(7),
    scale(1.0),
    _active(true),
    _model(dfmodel)
{
    CHECKED_CONNECT(this, SIGNAL(lightChanged(int,int,Lightcolor)),
        _model, SLOT(lightChanged(int,int,Lightcolor)));

}


void Cue::levelChanged(qreal level)
{
    if (!_active) return;
    qDebug() << "cue light Changed" << level  ;

    Lightcolor lightc = Lightcolor::LightcolorF(level, level, level);
    emit(lightChanged(xoffset, yoffset, lightc));
}
