#include "Remap.h"

Remap::Remap()  /*
    :
    _output(0.0),
    _minOut(0.0),
    _maxOut(1.0),
    _minIn(0.0),
    _maxIn(1.0)
    */
{
    setName("Remap");
    _type = FLOAT;

#ifndef OLDPARAMS
    // Declare params.
//    addParam("out", 1.0, true, true);
    addParam<float>("out", 0.0, true);
    addParam<float>("minOut", 0.0);
    addParam<float>("maxOut", 1.0);
    addParam<float>("minIn", 0.0);
    addParam<float>("maxIn", 1.0);
    addParam<float>("input", 0.0);

#else
    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _minOut.setName("minOut");
    _minOut.setOutput(false);
    _minOut.setConnectable(true);

    _maxOut.setName("maxOut");
    _maxOut.setOutput(false);
    _maxOut.setConnectable(true);

    _minIn.setName("minIn");
    _minIn.setOutput(false);
    _minIn.setConnectable(true);

    _maxIn.setName("maxIn");
    _maxIn.setOutput(false);
    _maxIn.setConnectable(true);

    _input.setName("input");
    _input.setOutput(false);
    _input.setConnectable(true);

    _paramList << &_output << &_minOut << &_maxOut << &_minIn << &_maxIn << &_input;
    setParamParent();
#endif
}

void Remap::operator()()
{
    // Boilerplate start-of-operator():
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

#ifndef OLDPARAMS
    float input;
    getValue("input", input);
    float minIn;
    getValue("minIn", minIn);
    float maxIn;
    getValue("maxIn", maxIn);
    float minOut;
    getValue("minOut", minOut);
    float maxOut;
    getValue("maxOut", maxOut);
    float output = 0.;

    if (input < minIn)
        output = minOut;
    else if (input > maxIn)
        output = maxOut;
    else {
        double frac = (input-minIn) / (maxIn-minIn);
        output = frac * (maxOut-minOut) + minOut;
    }
#else
    if (_input._value < _minIn._value)
        _output._value = _minOut._value;
    else if (_input._value > _maxIn._value)
        _output._value = _maxOut._value;
    else {
        double frac = (_input._value - _minIn._value) / (_maxIn._value - _minIn._value);
        _output._value = frac * (_maxOut._value - _minOut._value) + _minOut._value;
    }

    // Boilerplate end of operator:
    _output._qvOutput = _output._value;
#endif
    setValue("out", output);
}

Remap* Remap::clone()
{
    Remap* lhs = new Remap();
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}

static Registrar<Remap>   registrar1("Remap", Node::FLOAT);
