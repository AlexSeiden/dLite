#include "Cue.h"
#include "utils.h"
#include "lightcolor.h"
#include "dancefloormodel.h"

void randLight(int &value)
{
    value = rand() % 18;
}

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
    _xoffset.setProvider(randLight);
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

    Lightcolor lightc = Lightcolor::Lightcolor(alpha, alpha, alpha);
    _floor->setPixel(xoffset, yoffset, lightc);
}





void Cue::setAlphaProvider(providerFunctor_t &provider)
{
    _alpha.setProvider(provider);
}


#if 0
void Cue::setPropertyProvider(std::string propertyName, providerFunctor_t &provider)
{
    Param *param = getParam(propertyName);
    param.setProvider(provider);
}

Param *
Cue::getParam(std::string propertyName)
{

}
#endif
