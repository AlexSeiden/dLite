#include "DotCue.h"
#include "DanceFloor.h"
#include <QDebug>
#include "Node.h"
#include "NodeFactory.h"

DotCue::DotCue()
{
    int numCues = m_dfModel->getNumCues();
    setName(QString("BoxCue%1").arg(numCues));
    m_type = CUE;

    // Declare params.
    addParam<int>("x", 10);
    addParam<int>("y", 8);
    addParam<float>("alpha", 1.0);
    addParam<Lightcolor>("color", Lightcolor(255,255,255));
}

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
    firing->m_color = color;
    firing->m_alpha = alpha;
    firing->setDecayMode(m_decayMode);
    firing->setCompMode(m_compMode);
    m_dfModel->fireLight(x, y, firing);
}


static Registrar<DotCue>     registrar("Dot", Node::CUE);
