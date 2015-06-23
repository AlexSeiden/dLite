#include <QDebug>
#include "Firing.h"

// -----------------------------------------------------------------------------
// Firing
// ??? do the decay functions want to be member functions of Firing,
// or free functions?  I'm thinking free functions, in that they might
// be independent of the state of the Firing, but perhaps not....
bool instantDecay(Firing *f) {
    Q_UNUSED(f);
    return false;
}

bool noDecay(Firing *f) {
    Q_UNUSED(f);
    return true;
}

bool exponentialDecay(Firing *f) {
    f->m_color *= .9;
    f->m_alpha *= .9;
    if (f->m_alpha < .01)
        return false;
    return true;
}

Firing::Firing() :
    m_color(),
    m_alpha(0.0),
    m_compMode(SET),
    m_decayfunction(exponentialDecay)
{ }

Firing::Firing(Lightcolor color, double alpha, compMode_t compmode,
               decayfunc_t decayfunc, Cue *cue) :
    m_color(color),
    m_alpha(alpha),
    m_compMode(compmode),
    m_decayfunction(decayfunc),
    m_cue(cue)
{ }

void Firing::setDecayMode(decayMode_t dmode) // GROSS
{
    if (dmode == IMMEDIATE)
        m_decayfunction = instantDecay;
    else if (dmode == FOREVER)
        m_decayfunction = noDecay;
    else if (dmode == EXPONENTIAL)
        m_decayfunction = exponentialDecay;      // TODO allow setting decay params
    else
        qDebug() << "setDecay called with bad arg " << dmode;
}

bool Firing::evaluate() {
    bool keep = true;
    if (m_decayfunction)
        keep = m_decayfunction(this);

    return keep;
    // When "keep" is false, this firing is removed from the list.
}

Lightcolor Firing::compOver(const Lightcolor &lightcolor) {
    Lightcolor out = lightcolor;
    out *= (1.0 - m_alpha);
    out += m_color;
    // TODO compute new alpha
    return out;
}


Lightcolor Firing::compAdd(const Lightcolor &lightcolor) {
    Lightcolor out = lightcolor;
    out += m_color;
    // TODO compute new alpha
    return out;
}

Lightcolor Firing::compSet(const Lightcolor &lightcolor) {
    Q_UNUSED(lightcolor)

    Lightcolor out = m_color;
    // TODO compute new alpha
    return out;
}
