#ifndef IMAGER_H
#define IMAGER_H
#include <QImage>
#include "Cue.h"

// ------------------------------------------------------------------------------
// Shape
//      Abstract base class for Cues that use a Qt Painter for drawing.

class Shape : public Cue
{
public:
    Shape();

    void    evaluate();
    void    operator() (void);
    virtual Shape* clone();

protected:
    virtual void    draw();
    virtual void    fire();
    QImage* m_image;
};


// ------------------------------------------------------------------------------
//      Draws a circle
class Circle : public Shape
{
public:
    Circle();
    virtual Circle* clone();

protected:
    void    draw();
};


// ------------------------------------------------------------------------------
// Box
//      Draws a rectangle
class Box : public Shape
{
public:
    Box();
    virtual Box* clone();

protected:
    void    draw();
};

#endif // IMAGER_H
