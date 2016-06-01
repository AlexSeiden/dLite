// Copyright (C) 2014-2016 Alex Seiden
//
#include "RegionCue.h"
#include "engine/nodefactory.h"
#include "engine/dancefloor.h"

RegionCue::RegionCue()
{
    setName(QString("RegionCue"));
    m_type = CUE;

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
    getValue("region", region); // ??? how heavy is this-could optimize?

    // "Fire" the light with the correct parameters
    foreach (QPoint p, region.getCells()) {
        Firing *firing = new Firing;
        firing->m_color = color*alpha;
        firing->m_alpha = alpha;
        firing->setDecayMode(m_decayMode);
        firing->setCompMode(m_compMode);
        m_dfModel->fireLight(p.x(), p.y(), firing);
    }
}


static Registrar<RegionCue>     registrar("RegionCue", Node::CUE);
