#ifndef LIGHTCOLOR_H
#define LIGHTCOLOR_H

#include <vector>

enum compmode_t {SET, ADD, OVER, UNDER};

class Lightcolor
{
public:
    Lightcolor();
    Lightcolor(int r, int g, int b);
    explicit Lightcolor(float r, float g, float b);
//    static Lightcolor LightcolorF(float r, float g, float b);

    int getRed()    {return m_r;}
    int getGreen()  {return m_g;}
    int getBlue()   {return m_b;}

private:
    int m_r, m_g, m_b;
};

typedef bool (*decayfunc_t)();

class Firing
{
public:
    Firing();
    //Firing(Lightcolor color, float alpha, compmode_t compmode, void (*decayfunc)());
    Firing(Lightcolor color, float alpha, compmode_t compmode, decayfunc_t decayfunc);

private:
    Lightcolor  _color;
    float       _alpha;
    compmode_t  _compMode;
    decayfunc_t _decayfunction;
};

class Light
{
public:
    Light();

    Lightcolor          _value;
    int                 _lightID;
    std::vector <Firing *>   _firings;
};

#endif // LIGHTCOLOR_H
