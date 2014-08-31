#include "CueBox.h"

void randLight(int &value); // XXX

CueBox::CueBox(Dancefloormodel *dfmodel) :
    Cue(dfmodel),
    _x(10),
    _y(8),
    _scale(1.0),
    _alpha(1.0),
    _color(Lightcolor(255,255,255))
{
    int numCues = _dfModel->getNumCues();
    setName(QString("BoxCue%1").arg(numCues));

    _x.setProvider(randLight); // XXX testing
    _y.setProvider(randLight); // XXX testing
}


void CueBox::evaluate()
{
    if (!_active) return;

    // Evaluate all attachable params:
    int x=0;
    int y=0;
    Lightcolor color;
    float alpha=1.0;
    _x.getValue(x);
    _y.getValue(y);
    _color.getValue(color);
    _alpha.getValue(alpha);

    // "Fire" the light with the correct parameters
    Firing *firing = new Firing;
    firing->_color = color;
    firing->_alpha = alpha;
    _dfModel->fireLight(x, y, firing);
}
