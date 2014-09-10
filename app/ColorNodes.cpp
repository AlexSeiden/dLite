#include "ColorNodes.h"

// ------------------------------------------------------------------------------
//  Color ramp
//      Mix between two colors
ColorRamp::ColorRamp() :
    Node(),
    _output(0),
    _c0(0),
    _c1(1),
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
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    Lightcolor part0, part1;
    part0 = _c0._value*(1.0-_mix._value);     // NOTE Doesn't seem to compile if float is first.
    part1 = _c1._value*_mix._value;

//    _value = (1.0-t)*c0 + t*c1;
    _output._value = part0 + part1;
    _output._qvOutput.setValue(_output._value);
}

static Registrar<ColorRamp>     registrar("ColorRamp", Node::COLOR);
