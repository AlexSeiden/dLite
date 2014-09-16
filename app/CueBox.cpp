#include "CueBox.h"
#include "DanceFloor.h"
#include <QDebug>
#include "Node.h"

Dancefloor *Cue::_dfModel = nullptr;

CueBox::CueBox() :
    _x(10),
    _y(8),
    _scale(1.0),
    _alpha(1.0),
    _color(Lightcolor(255,255,255))
{
    int numCues = _dfModel->getNumCues();
    setName(QString("BoxCue%1").arg(numCues));
    _type = CUE;

    // Declare params.
    _x.setName("x");
    _x.setOutput(false);
    _x.setConnectable(true);

    _y.setName("y");
    _y.setOutput(false);
    _y.setConnectable(true);

    _scale.setName("scale");
    _scale.setOutput(false);
    _scale.setConnectable(true);

    _alpha.setName("alpha");
    _alpha.setOutput(false);
    _alpha.setConnectable(true);

    _color.setName("color");
    _color.setOutput(false);
    _color.setConnectable(true);

    _paramList << &_x <<&_y <<&_scale << &_alpha << &_color;
    setParamParent();
}

// Is this actually called?  I don't think so, but I guess since Node is
// pure virtual and operator() must be defined so that the class is concrete,
// this is here...
void CueBox::operator()() {
    evaluate();
}

void CueBox::evaluate()
{
    if (!_active) return;

    evalAllInputs();

    Lightcolor color;
    float alpha;
    int x,y;

    _color.getValue(color);
    _alpha.getValue(alpha);
    _x.getValue(x);
    _y.getValue(y);
    color *= alpha;

    // "Fire" the light with the correct parameters
    Firing *firing = new Firing;
    firing->_color = color;
    firing->_alpha = alpha;
    firing->setDecayMode(_decayMode);
    firing->setCompMode(_compMode);
    _dfModel->fireLight(x, y, firing);
}


static Registrar<CueBox>     registrar("BoxCue", Node::CUE);
