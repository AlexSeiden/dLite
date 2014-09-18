#include "ColorNodes.h"

// ------------------------------------------------------------------------------
//  Color ramp
//      Mix between two colors
ColorRamp::ColorRamp() :
    _output(0),
    _c0(0.0),
    _c1(1.0),
    _mix(.5)
{
    setName("ColorRamp");
    _type = COLOR;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _c0.setName("Color0");
    _c0.setOutput(false);
    _c0.setConnectable(true);

    _c1.setName("Color1");
    _c1.setOutput(false);
    _c1.setConnectable(true);

    _mix.setName("mix");
    _mix.setOutput(false);
    _mix.setConnectable(true);

    _paramList << &_output << &_c1 << &_c0 << &_mix;
    setParamParent();
}

void ColorRamp::operator()()
{
    // Boilerplate start of operator:
    // TODO:  add start & end boilerplate to the generic
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    Lightcolor part0, part1;
    part0 = (1.0-_mix._value)*_c0._value;
    part1 = _c1._value*_mix._value;
    _output._value = part0 + part1;

    // Boilerplate end of operator:
    _output._qvOutput.setValue(_output._value);
}

// ------------------------------------------------------------------------------
//  BriteColor
//      Creates a new hi-brightness, hi-sat color every trigger
BriteColor::BriteColor() :
    _output(0)
{
    setName("BriteColor");
    _type = COLOR;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _trigger.setName("trigger");
    _trigger.setOutput(false);
    _trigger.setConnectable(true);

    _paramList << &_output << &_trigger;
    setParamParent();
    setRandomEngine();
}

void BriteColor::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());

    // Initialize the distrubution on the range [_min, _max)
    // NOTE:  this is the HALF-OPEN interval--inclusive of min, but exclusive of max
    // XXX if these are connections, this is going to break.
#if 0
    float minSat, maxSat;
    _min.getValue(minSat);
    _max.getValue(maxSat);
    _distSat = new std::uniform_real_distribution<float>(minSat, maxSat);
#else
    _distSat = new std::uniform_real_distribution<float>(.7, 1.0);
#endif
    _distHue = new std::uniform_real_distribution<float>(0.0, 1.0);
    _distVal = new std::uniform_real_distribution<float>(0.8, 1.0);
}

void BriteColor::operator()()
{
    // Boilerplate start of operator:
    // TODO:  add start & end boilerplate to the generic
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    if (_trigger._value) {
        QColor out;
        double sat = (*_distSat)(*_randGenerator);
        double hue = (*_distHue)(*_randGenerator);
        double val = (*_distVal)(*_randGenerator);
        out.setHsvF(hue,sat,val);
        _output._value = out;
    }

    // Boilerplate end of operator:
    _output._qvOutput.setValue(_output._value);
}

static Registrar<BriteColor>     registrar0("BriteColor", Node::COLOR);
static Registrar<ColorRamp>      registrar1("ColorRamp", Node::COLOR);
