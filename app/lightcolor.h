#ifndef LIGHTCOLOR_H
#define LIGHTCOLOR_H

#include <vector>
#include <QColor>
#include <QMetaType>

// Forward declarations
class Cue;
class Firing;

enum compMode_t {SET, ADD, OVER, UNDER};
enum decayMode_t {IMMEDIATE, EXPONENTIAL, FOREVER};

// -----------------------------------------------------------------------------
// Lightcolor

class Lightcolor
{
public:
    Lightcolor();
    ~Lightcolor();

    Lightcolor(int r, int g, int b);
    Lightcolor(int val);
    Lightcolor(double r, double g, double b);
    Lightcolor(double val);
    Lightcolor(const QColor &qc);
    Lightcolor(const QRgb &qrgb);
    Lightcolor(const Lightcolor &rhs);

    int getRed() const    {return m_r;}
    int getGreen() const  {return m_g;}
    int getBlue() const   {return m_b;}

    Lightcolor  & operator=(const Lightcolor &rhs);

    Lightcolor  & operator+=(const Lightcolor &rhs);
    Lightcolor  & operator*=(const Lightcolor &rhs);
    Lightcolor  & operator*=(double scalar);

    const Lightcolor  operator*(double rhs);
//    const Lightcolor  operator*(const Lightcolor &rhs);
    const Lightcolor  operator+(const Lightcolor &rhs);

    QColor toQColor();

private:
    int m_r, m_g, m_b;
};

// Non-member operator allows implicit type promotion.
// See ColorRamp node for an example.
const Lightcolor operator*(const Lightcolor& lhs, const Lightcolor& rhs);

Q_DECLARE_METATYPE(Lightcolor)

// -----------------------------------------------------------------------------
// Light

class Light
{
public:
    Light();

    int                 _lightID;
    Lightcolor          _value;
    std::vector <Firing *>   _firings;
};

#endif // LIGHTCOLOR_H
