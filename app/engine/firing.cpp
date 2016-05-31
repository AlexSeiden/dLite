#include <QDebug>
#include "engine/firing.h"

// -----------------------------------------------------------------------------
// Firing
// ??? do the decay functions want to be member functions of Firing,
// or free functions?  I'm thinking free functions, in that they might
// be independent of the state of the Firing, but perhaps not....
bool immediateDecay(Firing *f) {
    Q_UNUSED(f);
    return false;
}

bool noDecay(Firing *f) {
    Q_UNUSED(f);
    return true;
}

bool exponentialDecay(Firing *f) {
    f->m_color *= f->m_decayRate;
    f->m_alpha *= f->m_decayRate;
    // TODO should check alpha and all channels against a non-hardwired threshold.
    if (f->m_alpha < .01)
        return false;
    return true;
}

Firing::Firing() :
    m_color(),
    m_alpha(0.0),
    m_compMode(SET),
    m_decayFunction(immediateDecay),
    m_decayRate(0.9),
    m_renderNode(nullptr)
{ }

Firing::Firing(Lightcolor color, double alpha, compMode_t compmode,
               decayfunc_t decayFunc, float decayRate, RenderNode *renderNode) :
    m_color(color),
    m_alpha(alpha),
    m_compMode(compmode),
    m_decayFunction(decayFunc),
    m_decayRate(decayRate),
    m_renderNode(renderNode)
{ }

void Firing::setDecayMode(decayMode_t dmode)
{
    if (dmode == IMMEDIATE)
        m_decayFunction = immediateDecay;
    else if (dmode == FOREVER)
        m_decayFunction = noDecay;
    else if (dmode == EXPONENTIAL)
        m_decayFunction = exponentialDecay;
    else
        qDebug() << "setDecay called with bad arg " << dmode;
}

bool Firing::evaluate() {
    bool keep = true;
    if (m_decayFunction)
        keep = m_decayFunction(this);

    return keep;
    // When "keep" is false, this firing is removed from the list.
}

Lightcolor Firing::compOver(const Lightcolor &lightcolor) {
    Lightcolor out = lightcolor;
    out *= (1.0 - m_alpha);
    out += m_color;
    // TODO compute & return new alpha
    return out;
}

Lightcolor Firing::compAdd(const Lightcolor &lightcolor) {
    Lightcolor out = lightcolor;
    out += m_color;
    // TODO compute & return new alpha
    return out;
}

Lightcolor Firing::compSet(const Lightcolor &lightcolor) {
    Q_UNUSED(lightcolor)

    Lightcolor out = m_color;
    // TODO compute & return new alpha
    return out;
}
