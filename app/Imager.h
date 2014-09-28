#ifndef IMAGER_H
#define IMAGER_H
#include <QImage>
#include "Cue.h"

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
