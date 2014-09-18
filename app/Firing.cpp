#include "Firing.h"
#include <QDebug>

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
    f->_color *= .9;
    f->_alpha *= .9;
    if (f->_alpha < .01)
        return false;
    return true;
}

Firing::Firing() :
    _color(),
    _alpha(0.0),
    _compMode(SET),
    _decayfunction(exponentialDecay)
{ }

Firing::Firing(Lightcolor color, double alpha, compMode_t compmode, decayfunc_t decayfunc, Cue *cue) :
    _color(color),
    _alpha(alpha),
    _compMode(compmode),
    _decayfunction(decayfunc),
    _cue(cue)
{ }

void Firing::setDecayMode(decayMode_t dmode) // GROSS
{
    if (dmode == IMMEDIATE)
        _decayfunction = instantDecay;
    else if (dmode == FOREVER)
        _decayfunction = noDecay;
    else if (dmode == EXPONENTIAL)
        _decayfunction = exponentialDecay;      // TODO allow setting decay params
    else
        qDebug() << "setDecay called with bad arg " << dmode;
}

bool Firing::evaluate() {
    bool keep = true;
    if (_decayfunction)
        keep = _decayfunction(this);

    return keep;
    // When "keep" is false, this firing is removed from
    // the list.
}

Lightcolor Firing::compOver(const Lightcolor &lightcolor) {
    Lightcolor out = lightcolor;
    out *= (1.0 - _alpha);
    out += _color;
    // XXX compute new alpha
    return out;
}


Lightcolor Firing::compAdd(const Lightcolor &lightcolor) {
    Lightcolor out = lightcolor;
    out += _color;
    // XXX compute new alpha
    return out;
}

Lightcolor Firing::compSet(const Lightcolor &lightcolor) {
    Q_UNUSED(lightcolor)

    Lightcolor out = _color;
    // XXX compute new alpha
    return out;
}
