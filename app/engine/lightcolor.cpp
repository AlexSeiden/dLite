// Copyright (C) 2014-2016 Alex Seiden 
//
//  The class representing a color anywhere in the system.
//  I thought, "oh for sure I'll want my own color type cause
//  doubtless I'll do lots of totally unique stuff with it."
//  But it's really pretty boilerplate, and I could have used QtColor
//  and saved some code bloat. Sometime when I'm bored I'll probably
//  rip this out.

#include <QDebug>
#include <QRgb>

#include "engine/lightcolor.h"

// -----------------------------------------------------------------------------
//  Lightcolor

// Constructors
Lightcolor::Lightcolor() : m_r(0), m_g(0), m_b(0) { }
Lightcolor::~Lightcolor() { }

Lightcolor::Lightcolor(int r, int g, int b) : m_r(r), m_g(g), m_b(b) { }

Lightcolor::Lightcolor(int val) : m_r(val), m_g(val), m_b(val) { }

Lightcolor::Lightcolor(double r, double g, double b) :
    m_r(r*255), m_g(g*255), m_b(b*255) { }

Lightcolor::Lightcolor(double val) : m_r(val*255), m_g(val*255), m_b(val*255) { }

Lightcolor::Lightcolor(const QRgb &qrgb) :
    m_r(qRed(qrgb)), m_g(qGreen(qrgb)), m_b(qBlue(qrgb)) { }

Lightcolor::Lightcolor(const QColor &qc) :
    m_r(qc.red()), m_g(qc.green()), m_b(qc.blue()) { }

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

Lightcolor& Lightcolor::operator=(const Lightcolor& rhs) // copy assignment
{
    if (this != &rhs) { // self-assignment check expected
        this->m_r = rhs.m_r;
        this->m_g = rhs.m_g;
        this->m_b = rhs.m_b;
    }
    return *this;
}

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

int clamp(int in) {
    if (in < 0) return 0;
    if (in > 255) return 255;
    return in;
}

void Lightcolor::setRGB(unsigned char *rgb)
{
    rgb[0] = clamp(m_r);
    rgb[1] = clamp(m_g);
    rgb[2] = clamp(m_b);
}


// -----------------------------------------------------------------------------
// Light

Light::Light() :
    m_lightID(0),
    m_value(Lightcolor())
{ }
