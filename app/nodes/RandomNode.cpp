// Copyright (C) 2014-2016 Alex Seiden
//
#include <QDebug>
#include "RandomNode.h"
#include "engine/nodefactory.h"

// ------------------------------------------------------------------------------
//  RandomFloat

RandomFloat::RandomFloat()
{
    setName("RandomFloat");
    m_type = FLOAT;

    // Declare params.
    addParam<float>("out", 0.0, true);
    addParam<float>("min", 0.0, false, false);
    addParam<float>("max", 1.0, false, false);
    addParam<bool>("trigger", true);

    setRandomEngine();
}

void RandomFloat::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());
    // Initialize the distrubution on the range [_min, _max)
    // NOTE:  this is the HALF-OPEN interval--inclusive of min, but exclusive of max
    float min, max;
    getValue("min", min);
    getValue("max", max);
    // XXX if these are connections, this isn't going to get reset at the correct time.
    _distribution = new std::uniform_real_distribution<float>(min, max);
}

void RandomFloat::paramHasBeenEdited()
{
    // Need to reset random engine.
    setRandomEngine();
}

void RandomFloat::operator()()
{
    // Boilerplate start-of-operator():
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    bool trigger;
    float out;
    getValue("trigger", trigger);
    getValue("out", out);

    if (trigger)
        out =  (*_distribution)(*_randGenerator);

    setValue("out", out);
}

RandomFloat* RandomFloat::clone()
{
    RandomFloat* lhs = new RandomFloat;
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
//  RandomInt

RandomInt::RandomInt()
{
    setName(QString("RandomInt%1").arg(nodeCount()));
    m_type = INT;

    // Declare params.
    addParam<float>("out", 0.0, true);
    addParam<float>("min", 0.0, false, false);
    addParam<float>("max", 1.0, false, false);
    addParam<bool>("trigger", true);

    // Specialized init for this node.  Will re-rerun if params edited.
    setRandomEngine();
}

void RandomInt::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());
    // Initialize the distrubution on the range [_min, _max]
    // NOTE:  this is the CLOSED interval--inclusive of both min and max.
    int min, max;
    getValue("min", min);
    getValue("max", max);
    // TODO re-run this when distributions change.
    // XXX if these are connections, this is going to break, but they
    // probably shouldn't be connectable.
    _distribution = new std::uniform_int_distribution<int>(min, max);
}

void RandomInt::paramHasBeenEdited()
{
    // Need to reset random engine.
    setRandomEngine();
}

void RandomInt::operator()()
{
    // Boilerplate start-of-operator():
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    bool trigger;
    int out;
    getValue("trigger", trigger);
    getValue("out", out);

    // First, check the trigger to see if it's time for a new number
    if (trigger)
        out =  (*_distribution)(*_randGenerator);

    setValue("out", out);
}

RandomInt* RandomInt::clone()
{
    RandomInt* lhs = new RandomInt();
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
//  SequenceInt
//      Takes a trigger, and every time increments between _min and _max value.

SequenceInt::SequenceInt()
{
    setName(QString("SequenceInt%1").arg(nodeCount()));
    m_type = INT;

    // Declare params.
    addParam<int>("out", 0, true);
    addParam<int>("min", 6);
    addParam<int>("max", 12);
    addParam<int>("step", 1);
    addParam<bool>("trigger", true);

    getValue("min", _counter);
}

void SequenceInt::paramHasBeenEdited()
{
    int min, max;
    getValue("min", min);
    getValue("max", max);

    // Need to recheck range.
    if (_counter > max)
        _counter = max;
    else if (_counter < min)
        _counter = min;
}

void SequenceInt::operator()()
{
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
        //XXX would need to recheck bounds if min or max changed.
    evalAllInputs();

    bool trigger;
    int min, max, step;
    int out;
    getValue("trigger", trigger);
    getValue("min", min);
    getValue("max", max);
    getValue("step", step);
    getValue("out", out);

    // First, check the trigger to see if it's time for a new number
    // XXX also breaks when scrubbing.
    if (trigger) {
        _counter += step;
        if (_counter > max)
            _counter = min;
        out = _counter;
    }

    setValue("out", out);
}

SequenceInt* SequenceInt::clone()
{
    SequenceInt* lhs = new SequenceInt;
    cloneHelper(*lhs);
    return lhs;
}

// ------------------------------------------------------------------------------
//  IntToFloat
//      Converts an int to a float

IntToFloat::IntToFloat()
{
    setName(QString("IntToFloat%1").arg(nodeCount()));
    m_type = INT;

    // Declare params.
    addParam<float>("out", 0., true);
    addParam<int>("in", 0);
}

void IntToFloat::paramHasBeenEdited()
{
    int in;
    getValue("in", in);
}

void IntToFloat::operator()()
{
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    int in;
    getValue("in", in);
    float out = in;
    setValue("out", out);
}

IntToFloat* IntToFloat::clone()
{
    IntToFloat* lhs = new IntToFloat;
    cloneHelper(*lhs);
    return lhs;
}

static Registrar<RandomFloat>   registrar1("RandomFloat",   Node::FLOAT);
static Registrar<RandomInt>     registrar2("RandomInt",     Node::INT);
static Registrar<SequenceInt>   registrar3("SequenceInt",   Node::INT);
static Registrar<IntToFloat>    registrar4("IntToFloat",    Node::FLOAT);
