#include "lightcolor.h"

Lightcolor::Lightcolor() : m_r(0), m_g(0), m_b(0)
{ }

Lightcolor::Lightcolor(int r, int g, int b) : m_r(r), m_g(g), m_b(b)
{ }

Lightcolor::Lightcolor(int val) : m_r(val), m_g(val), m_b(val)
{ }

Lightcolor::Lightcolor(float r, float g, float b) : m_r(r*255), m_g(g*255), m_b(b*255)
{ }

Lightcolor::Lightcolor(float val) : m_r(val*255), m_g(val*255), m_b(val*255)
{ }

Lightcolor::Lightcolor(const QColor &qc) : m_r(qc.red()), m_g(qc.green()), m_b(qc.blue())
{ }

QColor
Lightcolor::toQColor() {
    int r = std::min(255, std::max(m_r,0));
    int g = std::min(255, std::max(m_g,0));
    int b = std::min(255, std::max(m_b,0));

    return QColor(r,g,b);
}

// Lightcolor operators
Lightcolor &
Lightcolor::operator*=(float scalar) {
    this->m_r *= scalar;
    this->m_g *= scalar;
    this->m_b *= scalar;
    return *this;
}

inline Lightcolor
operator+(Lightcolor lhs, const Lightcolor& rhs)
{
  lhs += rhs;
  return lhs;
}

inline Lightcolor
operator*(Lightcolor lhs, float rhs)
{
  lhs *= rhs;
  return lhs;
}

Lightcolor &
Lightcolor::operator+=(const Lightcolor &other) {
    this->m_r += other.m_r;
    this->m_g += other.m_g;
    this->m_b += other.m_b;
    return *this;
}


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

Firing::Firing(Lightcolor color, float alpha, compmode_t compmode, decayfunc_t decayfunc, Cue *cue) :
    _color(color),
    _alpha(alpha),
    _compMode(compmode),
    _decayfunction(decayfunc),
    _cue(cue)
{ }

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
    out *= _alpha;
    out += _color;
    return out;
}


// -----------------------------------------------------------------------------
// Light

Light::Light() :
    _lightID(0),
    _value(Lightcolor())
{ }

