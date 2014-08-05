#ifndef LIGHTCOLOR_H
#define LIGHTCOLOR_H

class Lightcolor
{
public:
    Lightcolor();
    Lightcolor(int r, int g, int b);
    static Lightcolor LightcolorF(float r, float g, float b);

    int getRed()    {return m_r;};
    int getGreen()  {return m_g;};
    int getBlue()   {return m_b;};

private:
    int m_r, m_g, m_b;
};

#endif // LIGHTCOLOR_H
