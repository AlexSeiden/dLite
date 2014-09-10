#include "lightcolor.h"
#include <QDebug>

Lightcolor::Lightcolor() : m_r(0), m_g(0), m_b(0) { }
Lightcolor::~Lightcolor() { }

Lightcolor::Lightcolor(int r, int g, int b) : m_r(r), m_g(g), m_b(b) { }

Lightcolor::Lightcolor(int val) : m_r(val), m_g(val), m_b(val) { }

Lightcolor::Lightcolor(float r, float g, float b) : m_r(r*255), m_g(g*255), m_b(b*255) { }

Lightcolor::Lightcolor(float val) : m_r(val*255), m_g(val*255), m_b(val*255) { }

Lightcolor::Lightcolor(const QColor &qc) : m_r(qc.red()), m_g(qc.green()), m_b(qc.blue()) { }

Lightcolor::Lightcolor(const Lightcolor &rhs)
{
    this->m_r = rhs.m_r;
    this->m_g = rhs.m_g;
    this->m_b = rhs.m_b;
}

QColor Lightcolor::toQColor() {
    int r = std::min(255, std::max(m_r,0));
    int g = std::min(255, std::max(m_g,0));
    int b = std::min(255, std::max(m_b,0));

    return QColor(r,g,b);
}

// Lightcolor operators

// Assignment operator
Lightcolor& Lightcolor::operator=(const Lightcolor& rhs) // copy assignment
{
    if (this != &rhs) { // self-assignment check expected
        this->m_r = rhs.m_r;
        this->m_g = rhs.m_g;
        this->m_b = rhs.m_b;
    }
    return *this;
}

Lightcolor & Lightcolor::operator*=(float scalar) {
    this->m_r *= scalar;
    this->m_g *= scalar;
    this->m_b *= scalar;
    return *this;
}

Lightcolor & Lightcolor::operator*=(const Lightcolor &rhs) {
    this->m_r *= rhs.m_r;
    this->m_g *= rhs.m_g;
    this->m_b *= rhs.m_b;
    // Normalize
    this->m_r /= (255*255);
    this->m_g /= (255*255);
    this->m_b /= (255*255);
    return *this;
}

Lightcolor & Lightcolor::operator+=(const Lightcolor &rhs) {
    this->m_r += rhs.m_r;
    this->m_g += rhs.m_g;
    this->m_b += rhs.m_b;
    return *this;
}

const Lightcolor Lightcolor::operator*(float rhs) {
    Lightcolor result = *this;
    result *= rhs;
    return result;
}

const Lightcolor Lightcolor::operator*(const Lightcolor &rhs) {
    Lightcolor result = *this;
    result *= rhs;
    return result;
}

const Lightcolor Lightcolor::operator+(const Lightcolor& rhs) {
    Lightcolor result = *this;
    result += rhs;
    return result;
}




// -----------------------------------------------------------------------------
// Firing
// TODO move firing to its own .h & .cpp?
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

Firing::Firing(Lightcolor color, float alpha, compmode_t compmode, decayfunc_t decayfunc, Cue *cue) :
    _color(color),
    _alpha(alpha),
    _compMode(compmode),
    _decayfunction(decayfunc),
    _cue(cue)
{ }

void Firing::setDecay(int i) // GROSS
{
    if (i == 0)
        _decayfunction = noDecay;
    else if (i == 1)
        _decayfunction = instantDecay;
    else if (i == 2)
        _decayfunction = exponentialDecay;
    else
        qDebug() << "setDecay called with bad arg " << i;
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
    return out;
}


// -----------------------------------------------------------------------------
// Light

Light::Light() :
    _lightID(0),
    _value(Lightcolor())
{ }

