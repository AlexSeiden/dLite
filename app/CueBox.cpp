#include "CueBox.h"

void randLight(int &value); // XXX

CueBox::CueBox(Dancefloormodel *dfmodel) :
    Cue(dfmodel),
    _xoffset(10),
    _yoffset(8),
    _scale(1.0),
    _alpha(1.0),
    _color(Lightcolor(255,255,255))
{
    // Give it a name
    int numCues = _dfModel->getNumCues();
    _name = QString("BoxCue%1").arg(numCues);

    _xoffset.setProvider(randLight); // XXX testing
    _yoffset.setProvider(randLight); // XXX testing
}


void CueBox::evaluate()
{
    if (!_active) return;

    // Evaluate all attachable params:
    int xoffset=0;
    int yoffset=0;
    Lightcolor color;
    _xoffset.getValue(xoffset);
    _yoffset.getValue(yoffset);
    _color.getValue(color);

    float alpha=1.0;
    _alpha.getValue(alpha);

    // "Fire" the light with the correct parameters
    Firing *firing = new Firing;
    firing->_color = color;
    firing->_alpha = alpha;
    _dfModel->fireLight(xoffset, yoffset, firing);
}
