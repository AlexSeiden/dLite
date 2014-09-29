#ifndef IMAGER_H
#define IMAGER_H
#include <QImage>
#include "Cue.h"

// ------------------------------------------------------------------------------
// Imager
//      Abstract base class for Cues that use a Qt Painter for drawing.

class Imager : public Cue
{
public:
    Imager();

    void evaluate();
    void operator() (void);
    virtual Imager* clone();

protected:
    Param<float>        _alpha;
    Param<Lightcolor>   _color;

    virtual void    draw();
    virtual void    fire();
    QImage  *_image;
};

// ------------------------------------------------------------------------------
// Circle
//      Draws a circle
class Circle : public Imager
{
public:
    Circle();
    virtual Circle* clone();

protected:
    Param<float>        _x;
    Param<float>        _y;
    Param<float>        _scale;

    void    draw();
};

// ------------------------------------------------------------------------------
// Box
//      Draws a rectangle
class Box : public Imager
{
public:
    Box();
    virtual Box* clone();

protected:
    Param<float>        _x;
    Param<float>        _y;
    Param<float>        _width;
    Param<float>        _height;
    Param<float>        _rotation;      // XXX add rotation
    Param<bool>         _antialiased;

    void    draw();
};

#endif // IMAGER_H
