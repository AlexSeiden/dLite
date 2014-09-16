#ifndef LIGHTCOLOR_H
#define LIGHTCOLOR_H

#include <vector>
#include <QColor>
#include <QMetaType>

// Forward declarations
class Cue;

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

// Non-member operator allows implicit type promotion
const Lightcolor operator*(const Lightcolor& lhs, const Lightcolor& rhs);

Q_DECLARE_METATYPE(Lightcolor)

// -----------------------------------------------------------------------------
// Firing

class Firing;
typedef bool (*decayfunc_t)(Firing *);

class Firing
{
public:
    Firing();
    Firing(Lightcolor color, double alpha, compMode_t compmode, decayfunc_t decayfunc, Cue *cue=nullptr);

    bool        evaluate();

    Lightcolor  compOver(const Lightcolor &lightcolor);
    Lightcolor  compAdd(const Lightcolor &lightcolor);
    void        setCompMode(compMode_t cm) {_compMode = cm;}
//    void        setDecayMode(decayfunc_t dfunk) {_decayfunction = dfunk;}
    void        setDecayMode(decayMode_t dmode);

    Lightcolor  _color;
    float       _alpha;
    compMode_t  _compMode;
    decayfunc_t _decayfunction;
    Cue         *_cue; // mainly used when the same cue wants to replace
                      // a firing in the buffer, rather than add to it.
};

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
