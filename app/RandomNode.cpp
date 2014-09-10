#include "RandomNode.h"
#include <QDebug>

// ------------------------------------------------------------------------------
//  RandomFloat

RandomFloat::RandomFloat() :
    Node(),
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
    // TODO re-run this when distributions change.
    // XXX if these are connections, this is going to break.
    // Do we want a "non-animating" parameter class, or flag?
    _distribution = new std::uniform_real_distribution<float>(min, max);
}

void RandomFloat::paramHasBeenEdited()
{
    // Need to reset random engine.
    setRandomEngine();
}

void RandomFloat::operator()()
{
    // First, check the trigger to see if it's time for a new number
    bool trigVal = true;
    //XXX _trigger(trigVal);
    if (trigVal) {
        _output._value =  (*_distribution)(*_randGenerator);
    }
    _output._qvOutput = _output._value;
}

// ------------------------------------------------------------------------------
//  RandomInt

RandomInt::RandomInt() :
    Node(),
    _output(0),
    _min(0),
    _max(20),
    _trigger(true)
{
    setName(QString("RandomInt%1").arg(_nodeCount));
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
    // Do we want a "non-animating" parameter class, or flag?
    _distribution = new std::uniform_int_distribution<int>(min, max);
}

void RandomInt::paramHasBeenEdited()
{
    // Need to reset random engine.
    setRandomEngine();
}

void RandomInt::operator()()
{
    if (evaluatedThisFrame())
        return;
    evalAllInputs();

    // First, check the trigger to see if it's time for a new number
    if (_trigger._value) {
        _output._value =  (*_distribution)(*_randGenerator);
    }
    _output._qvOutput = _output._value;
}

static Registrar<RandomFloat>   registrar("RandomFloat", Node::FLOAT);
static Registrar<RandomInt>     registrar2("RandomInt", Node::INT);
