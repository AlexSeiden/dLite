#include "Node.h"
#include "utils.h"

Node::Node() :
    _active(true)
{ }

Node::~Node() {}


// ------------------------------------------------------------------------------
//  TriggerEvery
//  Impulse class

void TriggerEvery::operator ()(bool &value)
{
    if (getCurrentTime() > _nextRefresh) {
       _value = true;
       _lastRefresh = _nextRefresh;
       _nextRefresh += _interval;
    }
    value = _value;

    // Clear a "true" value after it's been read.
    // XXX This assumes that this node is only connected to one output;
    _value = false;
}


// ------------------------------------------------------------------------------
//  RandomNode
//  float & int classes

RandomNode::RandomNode() :
    _value(0.0),
    _output(0.0),
    _min(0.0),
    _max(1.0)
{
    setRandomEngine();
    setName("RandomFloat");
    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _min.setName("min");
    _min.setOutput(false);
    _min.setConnectable(true);

    _max.setName("max");
    _max.setOutput(false);
    _max.setConnectable(true);

    _paramList << &_output << &_min << &_max;
}

void RandomNode::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());
    // Initialize the distrubution on the range [_min, _max)
    float min, max;
    _min.getValue(min);
    _max.getValue(max);
    // TODO re-run this when distributions change.
    // XXX if these are connections, this is going to break.
    // Do we want a "non-animating" parameter class, or flag?
    _distribution = new std::uniform_real_distribution<float>(min, max);
}


void RandomNode::operator ()(float &value)
{
    // First, check the trigger to see if it's time for a new number
    bool trigVal = true;
    //XXX _trigger(trigVal);
    if (trigVal) {
        _value =  (*_distribution)(*_randGenerator);
    }

    value = _value;
}
