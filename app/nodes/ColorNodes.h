// Copyright (c) Alex Seiden 2014-2016
//
#ifndef COLORNODES_H
#define COLORNODES_H
#include <random>
#include "nodes/Node.h"

class ColorRamp : public Node
{
public:
    ColorRamp();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() ();
    virtual ColorRamp* clone();
};


class BriteColor : public Node
{
public:
    BriteColor();
    void operator() ();
    virtual BriteColor* clone();
    void paramHasBeenEdited();

private:
    void  setRandomEngine();
    std::mt19937 *_randGenerator;
    std::uniform_real_distribution<float> *m_distHue;
    std::uniform_real_distribution<float> *m_distSat;
    std::uniform_real_distribution<float> *m_distVal;
};


class Palette : public Node
{
public:
    Palette();
    void operator() ();
    virtual Palette* clone();
};

#endif // COLORNODES_H
