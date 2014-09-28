#include "Remap.h"

Remap::Remap() :
    _output(0.0),
    _minOut(0.0),
    _maxOut(1.0),
    _minIn(0.0),
    _maxIn(1.0)
{
    setName("Remap");
    _type = FLOAT;

    // Declare params.
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
}

void Remap::operator()()
{
    // Boilerplate start-of-operator():
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

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
}

Remap* Remap::clone()
{
    Remap* lhs = new Remap();
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}

static Registrar<Remap>   registrar1("Remap", Node::FLOAT);
