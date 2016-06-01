// Copyright (C) 2014-2016 Alex Seiden
//
// Remaps a float, kinda like the levels control in photoshop.

#include "nodes/RemapperNode.h"
#include "engine/nodefactory.h"

RemapperNode::RemapperNode()
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

void RemapperNode::operator()()
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

RemapperNode* RemapperNode::clone()
{
    RemapperNode* lhs = new RemapperNode();
    cloneHelper(*lhs);
    return lhs;
}

static Registrar<RemapperNode>   registrar1("Remap", Node::FLOAT);
