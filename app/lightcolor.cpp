#include "lightcolor.h"
#include <QDebug>
#include <QRgb>

// -----------------------------------------------------------------------------
//  Lightcolor

// Constructors
Lightcolor::Lightcolor() : m_r(0), m_g(0), m_b(0) { }
Lightcolor::~Lightcolor() { }

Lightcolor::Lightcolor(int r, int g, int b) : m_r(r), m_g(g), m_b(b) { }

Lightcolor::Lightcolor(int val) : m_r(val), m_g(val), m_b(val) { }

Lightcolor::Lightcolor(double r, double g, double b) : m_r(r*255), m_g(g*255), m_b(b*255) { }

Lightcolor::Lightcolor(double val) : m_r(val*255), m_g(val*255), m_b(val*255) { }

Lightcolor::Lightcolor(const QRgb &qrgb) : m_r(qRed(qrgb)), m_g(qGreen(qrgb)), m_b(qBlue(qrgb)) { }

Lightcolor::Lightcolor(const QColor &qc) : m_r(qc.red()), m_g(qc.green()), m_b(qc.blue()) { }

// Copy constructor
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

// Arithmetic  operators
Lightcolor & Lightcolor::operator*=(double scalar) {
    this->m_r *= scalar;
    this->m_g *= scalar;
    this->m_b *= scalar;
    return *this;
}

Lightcolor & Lightcolor::operator*=(const Lightcolor &rhs) {
    this->m_r *= rhs.m_r;
    this->m_g *= rhs.m_g;
    this->m_b *= rhs.m_b;
    // Normalize:
    // Done in float, this would be
    // float resultF = (r0/255.)*(r1/255.)
    // int resultI = resultF * 255;
    this->m_r /= 255;
    this->m_g /= 255;
    this->m_b /= 255;
    return *this;
}

Lightcolor & Lightcolor::operator+=(const Lightcolor &rhs) {
    this->m_r += rhs.m_r;
    this->m_g += rhs.m_g;
    this->m_b += rhs.m_b;
    return *this;
}

const Lightcolor Lightcolor::operator*(double rhs) {
    Lightcolor result = *this;
    result *= rhs;
    return result;
}

#if 0
// Now handled by non-member operator*, which allows implicit type promotion
// (I think...or maybe I've just fixed other problems....)
const Lightcolor Lightcolor::operator*(const Lightcolor &rhs) {
    Lightcolor result = *this;
    result *= rhs;
    return result;
}
#endif

const Lightcolor Lightcolor::operator+(const Lightcolor& rhs) {
    Lightcolor result = *this;
    result += rhs;
    return result;
}

// Allow mixed mode multiplication with implicit type conversions
// and arbitrary order (Myers, item 24)
const Lightcolor operator*(const Lightcolor& lhs, const Lightcolor& rhs)
{
    Lightcolor result = lhs;
    result *= rhs;
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


// -----------------------------------------------------------------------------
// Light

Light::Light() :
    _lightID(0),
    _value(Lightcolor())
{ }

