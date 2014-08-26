#include "Cue.h"
#include "utils.h"
#include "lightcolor.h"
#include "dancefloormodel.h"

void randLight(int &value)
{
    value = rand() % 18;
}

Cue::Cue(Dancefloormodel *dfmodel) :
    _dfModel(dfmodel),
    _active(true),
    _compmode(SET),
    _xoffset(10),
    _yoffset(8),
    _scale(1.0),
    _alpha(1.0),
    _color(255,255,255)
{
    // Add this to list of cues the model knows about.
    _dfModel->addCue(this);
    int numCues = _dfModel->getNumCues();
    _name = QString("BoxCue%1").arg(numCues);

    _xoffset.setProvider(randLight); // XXX testing
    _yoffset.setProvider(randLight); // XXX testing
}


void Cue::evaluate()
{
    if (!_active) return;

    // Evaluate all attachable params:
    int xoffset=0;
    int yoffset=0;
    _xoffset.getValue(xoffset);
    _yoffset.getValue(yoffset);

    float alpha=1.0;
    _alpha.getValue(alpha);

    Firing *firing = new Firing;
    firing->_color = _color;
    firing->_alpha = alpha;
    _dfModel->fireLight(xoffset, yoffset, firing);
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
