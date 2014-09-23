#include "RegionCue.h"
#include "DanceFloor.h"

RegionCue::RegionCue() :
    _color(Lightcolor(255,255,255)),
    _alpha(1.0)
{
    setName(QString("RegionCue"));
    _type = CUE;

    // Declare params.
    _color.setName("color");
    _color.setOutput(false);
    _color.setConnectable(true);

    _alpha.setName("alpha");
    _alpha.setOutput(false);
    _alpha.setConnectable(true);

    _region.setName("region");
    _region.setOutput(false);
    _region.setConnectable(true);

    _paramList << &_color << &_alpha << &_region;
    setParamParent();
}

void RegionCue::operator()() {
    evaluate();
}

void RegionCue::evaluate()
{
    if (!_active) return;

    evalAllInputs();

    Lightcolor color;
    float alpha;
    _color.getValue(color);
    _alpha.getValue(alpha);

    // "Fire" the light with the correct parameters
    foreach (QPoint p, _region._value.getCells()) {
        Firing *firing = new Firing;
        firing->_color = color*alpha;
        firing->_alpha = alpha;
        firing->setDecayMode(_decayMode);
        firing->setCompMode(_compMode);
        _dfModel->fireLight(p.x(), p.y(), firing);
    }
}


static Registrar<RegionCue>     registrar("RegionCue", Node::CUE);
