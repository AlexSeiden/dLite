// Copyright (C) 2014-2016 Alex Seiden
//
#ifndef SHAPE_NODE_H
#define SHAPE_NODE_H
#include <QImage>
#include "nodes/RenderNode.h"

// ------------------------------------------------------------------------------
// Shape
//      Abstract base class for Cues that use a Qt Painter for drawing.

class Shape : public RenderNode
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
//  Circle
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

#endif // SHAPE_NODE_H
