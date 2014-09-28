#include "RandomNode.h"
#include <QDebug>

// ------------------------------------------------------------------------------
//  RandomFloat

RandomFloat::RandomFloat() :
    _output(0.0),
    _min(0.0),
    _max(1.0),
    _trigger(true)
{
    setRandomEngine();
    setName("RandomFloat");
    _type = FLOAT;

    // Declare params.
    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _min.setName("min");
    _min.setOutput(false);
    _min.setConnectable(true);

    _max.setName("max");
    _max.setOutput(false);
    _max.setConnectable(true);

    _trigger.setName("trigger");
    _trigger.setOutput(false);
    _trigger.setConnectable(true);

    _paramList << &_output << &_min << &_max << &_trigger;
    setParamParent();
}

void RandomFloat::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());
    // Initialize the distrubution on the range [_min, _max)
    // NOTE:  this is the HALF-OPEN interval--inclusive of min, but exclusive of max
    float min, max;
    _min.getValue(min);
    _max.getValue(max);
    // XXX if these are connections, this is going to break.
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

    if (_trigger._value)
        _output._value =  (*_distribution)(*_randGenerator);

    _output._qvOutput = _output._value;
}

RandomFloat* RandomFloat::clone()
{
    RandomFloat* lhs = new RandomFloat;
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}

// ------------------------------------------------------------------------------
//  RandomInt

RandomInt::RandomInt() :
    _output(0),
    _min(0),
    _max(20),
    _trigger(true)
{
    setName(QString("RandomInt%1").arg(nodeCount()));
    _type = INT;

    // Declare params.
    _output.setName("out");
    _output.setConnectable(true);
    _output.setOutput(true);

    _min.setName("min");
    _min.setOutput(false);
    _min.setConnectable(false);
    // LATER: conceptually, the random min & max could be set by connections.
    // But for now, I'm gonna leave them unconnectable.

    _max.setName("max");
    _max.setOutput(false);
    _max.setConnectable(false);

    _trigger.setName("trigger");
    _trigger.setOutput(false);
    _trigger.setConnectable(true);

    _paramList << &_output << &_min << &_max << &_trigger;
    setParamParent();

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
    _min.getValue(min);
    _max.getValue(max);
    // TODO re-run this when distributions change.
    // XXX if these are connections, this is going to break.
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

    // First, check the trigger to see if it's time for a new number
    if (_trigger._value)
        _output._value =  (*_distribution)(*_randGenerator);

    // Boilerplate end of operator:
    _output._qvOutput = _output._value;
}

RandomInt* RandomInt::clone()
{
    RandomInt* lhs = new RandomInt();
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}

// ------------------------------------------------------------------------------
//  SequenceInt
//      Takes a trigger, and every time increments between _min and _max value.

SequenceInt::SequenceInt() :
    _output(0),
    _min(6),
    _max(12),
    _step(1),
    _trigger(true)
{
    setName(QString("SequenceInt%1").arg(nodeCount()));
    _type = INT;

    // Declare params.
    _output.setName("out");
    _output.setConnectable(true);
    _output.setOutput(true);

    _min.setName("min");
    _min.setOutput(false);
    _min.setConnectable(true);

    _max.setName("max");
    _max.setOutput(false);
    _max.setConnectable(true);

    _step.setName("step");
    _step.setOutput(false);
    _step.setConnectable(true);

    _trigger.setName("trigger");
    _trigger.setOutput(false);
    _trigger.setConnectable(true);

    _counter = _min._value;
    _paramList << &_output << &_min << &_max << &_step << &_trigger;
    setParamParent();
}

void SequenceInt::paramHasBeenEdited()
{
    // Need to recheck range.
    if (_counter > _max._value)
        _counter = _max._value;
    else if (_counter < _min._value)
        _counter = _min._value;
}

void SequenceInt::operator()()
{
    // Boilerplate start of operator:
    if (evaluatedThisFrame())
        return;
        //XXX would need to recheck bounds if min or max changed.
    evalAllInputs();

    // First, check the trigger to see if it's time for a new number
    // XXX also breaks when scrubbing.
    if (_trigger._value) {
        _counter += _step._value;
        if (_counter > _max._value)
            _counter = _min._value;
        _output._value =  _counter;
    }

    // Boilerplate end of operator:
    _output._qvOutput = _output._value;
}

SequenceInt* SequenceInt::clone()
{
    SequenceInt* lhs = new SequenceInt;
    cloneHelper(*lhs);
    setParamParent();
    return lhs;
}

static Registrar<RandomFloat>   registrar1("RandomFloat", Node::FLOAT);
static Registrar<RandomInt>     registrar2("RandomInt", Node::INT);
static Registrar<SequenceInt>   registrar3("SequenceInt", Node::INT);
