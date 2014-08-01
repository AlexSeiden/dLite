#include "lightcolor.h"

Lightcolor::Lightcolor() : m_r(0), m_g(0), m_b(0)
{
}

Lightcolor::Lightcolor(int r, int g, int b) : m_r(r), m_g(g), m_b(b)
{
}


#if 0
Lightcolor::Lightcolor(float r, float g, float b)
{
    m_r = r*255;
    m_g = g*255;
    m_b = b*255;
}
#endif

