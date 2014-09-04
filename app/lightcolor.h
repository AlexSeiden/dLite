#ifndef LIGHTCOLOR_H
#define LIGHTCOLOR_H

#include <vector>
#include <QColor>

// Forward declarations
class Cue;

enum compmode_t {SET, ADD, OVER, UNDER};

// -----------------------------------------------------------------------------
// Lightcolor

class Lightcolor
{
public:
    Lightcolor();
    Lightcolor(int r, int g, int b);
    Lightcolor(int val);
    explicit Lightcolor(float r, float g, float b);
    explicit Lightcolor(float val);
    explicit Lightcolor(const QColor &qc);

    int getRed()    {return m_r;}
    int getGreen()  {return m_g;}
    int getBlue()   {return m_b;}

    Lightcolor  & operator+=(const Lightcolor &rhs);
    Lightcolor  & operator*=(const Lightcolor &rhs);
    Lightcolor  & operator*=(float scalar);

    const Lightcolor  operator*(float rhs);
    const Lightcolor  operator*(const Lightcolor &rhs);
    const Lightcolor  operator+(const Lightcolor &rhs);

    QColor toQColor();

private:
    int m_r, m_g, m_b;
};


// -----------------------------------------------------------------------------
// Firing

class Firing;
typedef bool (*decayfunc_t)(Firing *);

class Firing
{
public:
    Firing();
    Firing(Lightcolor color, float alpha, compmode_t compmode, decayfunc_t decayfunc, Cue *cue=nullptr);

    bool        evaluate();
    //bool evaluate(long time);  // evaluates for a specific time?
    Lightcolor  getValue() {return _color;}
    float       getAlpha() {return _alpha;}
    Lightcolor  compOver(const Lightcolor &lightcolor);


//private:  //XXX
    Lightcolor  _color;
    float       _alpha;
    compmode_t  _compMode;
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
