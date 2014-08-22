#ifndef LIGHTCOLOR_H
#define LIGHTCOLOR_H

#include <vector>
#include <QColor>

enum compmode_t {SET, ADD, OVER, UNDER};

class Lightcolor
{
public:
    Lightcolor();
    Lightcolor(int r, int g, int b);
    Lightcolor(int val);
    explicit Lightcolor(float r, float g, float b);
    explicit Lightcolor(float val);

    int getRed()    {return m_r;}
    int getGreen()  {return m_g;}
    int getBlue()   {return m_b;}

    Lightcolor  & operator+=(const Lightcolor &other);
    Lightcolor  & operator*=(const Lightcolor &other);
    Lightcolor  & operator*=(float scalar);

    QColor toQColor();

private:
    int m_r, m_g, m_b;
};

Lightcolor  operator+(const Lightcolor lhs, const Lightcolor &rhs);
Lightcolor  operator*(const Lightcolor lhs, float rhs);

// -----------------------------------------------------------------------------
// Firing

class Firing;
typedef bool (*decayfunc_t)(Firing *);

class Firing
{
public:
    Firing();
    Firing(Lightcolor color, float alpha, compmode_t compmode, decayfunc_t decayfunc);

    bool        evaluate();
    //bool evaluate(long time);  // evaluates for a specific time?
    Lightcolor  getValue() {return _color;}
    float       getAlpha() {return _alpha;}
    Lightcolor  compOver(Lightcolor &lightcolor);


//private:  //XXX
    Lightcolor  _color;
    float       _alpha;
    compmode_t  _compMode;
    decayfunc_t _decayfunction;
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
