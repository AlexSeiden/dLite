#include "lightcolor.h"

Lightcolor::Lightcolor() : m_r(0), m_g(0), m_b(0)
{
}

Lightcolor::Lightcolor(int r, int g, int b) : m_r(r), m_g(g), m_b(b)
{
}

Lightcolor Lightcolor::LightcolorF(float r, float g, float b)
{
    return Lightcolor(r*255, g*255, b*255);
}
