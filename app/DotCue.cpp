#include "DotCue.h"
#include "DanceFloor.h"
#include <QDebug>
#include "Node.h"

DotCue::DotCue()
{
    int numCues = _dfModel->getNumCues();
    setName(QString("BoxCue%1").arg(numCues));
    _type = CUE;

    // Declare params.
    addParam<int>("x", 10);
    addParam<int>("y", 8);
    addParam<float>("alpha", 1.0);
    addParam<Lightcolor>("color", Lightcolor(255,255,255));
}

// Is this actually called?  I don't think so, but I guess since Node is
// pure virtual and operator() must be defined so that the class is concrete,
// this is here...
void DotCue::operator()() {
    evaluate();
}

DotCue* DotCue::clone()
{
    DotCue* lhs = new DotCue;
    cloneHelper(*lhs);
    return lhs;
}

void DotCue::evaluate()
{
    if (! isActive()) return;

    evalAllInputs();

    Lightcolor color;
    float alpha;
    int x,y;

    getValue("color", color);
    getValue("alpha", alpha);
    getValue("x", x);
    getValue("y", y);
    color *= alpha;

    // "Fire" the light with the correct parameters
    Firing *firing = new Firing;
    firing->_color = color;
    firing->_alpha = alpha;
    firing->setDecayMode(_decayMode);
    firing->setCompMode(_compMode);
    _dfModel->fireLight(x, y, firing);
}


static Registrar<DotCue>     registrar("Dot", Node::CUE);
