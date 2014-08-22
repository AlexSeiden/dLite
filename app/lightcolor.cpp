#include "lightcolor.h"

Lightcolor::Lightcolor() : m_r(0), m_g(0), m_b(0)
{
}

Lightcolor::Lightcolor(int r, int g, int b) : m_r(r), m_g(g), m_b(b)
{
}

Lightcolor::Lightcolor(float r, float g, float b) : m_r(r*255), m_g(g*255), m_b(b*255)
{
}

//Lightcolor Lightcolor::LightcolorF(float r, float g, float b)
//{
//    return Lightcolor(r*255, g*255, b*255);
//}

#if 0
bool
instantDecay()
{

}
#endif

Firing::Firing() :
    _color(),
    _alpha(0.0),
    _compMode(SET)
{

}
