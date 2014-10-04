#ifndef COLORNODES_H
#define COLORNODES_H
#include "Node.h"
#include <random>

class ColorRamp : public Node
{
public:
    ColorRamp();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() ();
    virtual ColorRamp* clone();

private:
    // Parameters
    Param<Lightcolor> _output;
    Param<Lightcolor> _c0;
    Param<Lightcolor> _c1;
    Param<float> _mix;
};


class BriteColor : public Node
{
public:
    BriteColor();
    void operator() ();
    virtual BriteColor* clone();
    void paramHasBeenEdited();

private:
    // Parameters
    Param<Lightcolor>   _output;
    Param<float>        _hueMin;
    Param<float>        _hueMax;
    Param<float>        _satMin;
    Param<float>        _satMax;
    Param<float>        _valMin;
    Param<float>        _valMax;
    Param<bool>         _trigger;

    void  setRandomEngine();
    std::mt19937 *_randGenerator;
    std::uniform_real_distribution<float> *_distHue;
    std::uniform_real_distribution<float> *_distSat;
    std::uniform_real_distribution<float> *_distVal;
};

class Palette : public Node
{
public:
    Palette();
    void operator() ();
    virtual Palette* clone();

private:
    // Parameters
    Param<Lightcolor> _output;
    Param<int> _selector;
    QList<Param<Lightcolor> * > _allcolors;

};

#endif // COLORNODES_H
