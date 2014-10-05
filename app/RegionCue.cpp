#include "RegionCue.h"
#include "DanceFloor.h"

RegionCue::RegionCue()
{
    setName(QString("RegionCue"));
    _type = CUE;

    // Declare params.
    addParam<Lightcolor>("color", Lightcolor(255,255,255));
    addParam<float>("alpha", 1.0);
    addParam<Region>("region", Region());
}

void RegionCue::operator()() {
    evaluate();
}

RegionCue* RegionCue::clone()
{
    RegionCue* lhs = new RegionCue;
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}

void RegionCue::evaluate()
{
    if (!isActive()) return;

    evalAllInputs();

    Lightcolor color;
    float alpha;
    getValue("color", color);
    getValue("alpha", alpha);
    Region region;
    getValue("region", region); // XXX how heavy is this????

    // "Fire" the light with the correct parameters
    foreach (QPoint p, region.getCells()) {
        Firing *firing = new Firing;
        firing->_color = color*alpha;
        firing->_alpha = alpha;
        firing->setDecayMode(_decayMode);
        firing->setCompMode(_compMode);
        _dfModel->fireLight(p.x(), p.y(), firing);
    }
}


static Registrar<RegionCue>     registrar("RegionCue", Node::CUE);
