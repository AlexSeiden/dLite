#include "Remap.h"
#include "NodeFactory.h"

Remap::Remap()
{
    setName("Remap");
    m_type = FLOAT;

    // Declare params.
    addParam<float>("out", 0.0, true);
    addParam<float>("minOut", 0.0);
    addParam<float>("maxOut", 1.0);
    addParam<float>("minIn", 0.0);
    addParam<float>("maxIn", 1.0);
    addParam<float>("input", 0.0);
}

void Remap::operator()()
{
    // Boilerplate start-of-operator():
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    float input;
    float minIn;
    float maxIn;
    float minOut;
    float maxOut;
    getValue("input", input);
    getValue("minIn", minIn);
    getValue("maxIn", maxIn);
    getValue("minOut", minOut);
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

    setValue("out", output);
}

Remap* Remap::clone()
{
    Remap* lhs = new Remap();
    cloneHelper(*lhs);
    return lhs;
}

static Registrar<Remap>   registrar1("Remap", Node::FLOAT);
