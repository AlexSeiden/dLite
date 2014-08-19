#include "cue.h"
#include "utils.h"
#include "lightcolor.h"
#include "dancefloormodel.h"

Cue::Cue(Dancefloormodel *dfmodel) :
    _floor(dfmodel),
    _active(true),
    _compmode(SET),
    _xoffset(10),
    _yoffset(7),
    _scale(1.0),
    _alpha(1.0),
    _color(255,255,255)
{
}


void Cue::evaluate()
{
    if (!_active) return;

    // Evaluate all attachable params:
    int xoffset=0;
    int yoffset=0;
    _xoffset.getValue(xoffset);
    _yoffset.getValue(yoffset);

    float alpha=0.0;
    _alpha.getValue(alpha);

    Lightcolor lightc = Lightcolor::LightcolorF(alpha, alpha, alpha);
    _floor->setPixel(xoffset, yoffset, lightc);
}
