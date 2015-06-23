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

    void    evaluate();
    void    operator() (void);
    virtual Imager* clone();

protected:
    virtual void    draw();
    virtual void    fire();
    QImage* m_image;
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
    void    draw();
};

#endif // IMAGER_H
