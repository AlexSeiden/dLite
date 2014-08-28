#include "Cue.h"
#include "utils.h"
#include "lightcolor.h"
#include "dancefloormodel.h"

void randLight(int &value)
{
    value = rand() % 18;
}

Cue::Cue(Dancefloormodel *dfmodel) :
    _dfModel(dfmodel),
    _active(true),
    _compmode(SET)
{
    // Add this to list of cues the model knows about.
    _dfModel->addCue(this);
}


void Cue::evaluate()
{
    if (!_active) return;
}


void
TriggerEvery::operator ()(bool &value){
    if (getCurrentTime() > _nextRefresh) {
       _value = true;
       _lastRefresh = _nextRefresh;
       _nextRefresh += _interval;
    }
    value = _value;

    // XXX This assumes that this node is only connected to one output
    _value = false;
}


RandomNode::RandomNode() :
    _value(0.0),
    _min(0.0),
    _max(1.0)
{
    setRandomEngine();
}

void
RandomNode::setRandomEngine()
{
    std::random_device rd;
    // Seed the "Mersenne Twister" random number generator from random_device
    _randGenerator = new std::mt19937(rd());
    // Initialize the distrubution on the range [_min, _max)
    _distribution = new std::uniform_real_distribution<float>(_min, _max);
}


void
RandomNode::operator ()(float &value){
    // First, check to see if it's time for a new number
    bool trigVal;
    _trigger(trigVal);
    if (trigVal) {
        _value =  (*_distribution)(*_randGenerator);
    }

    value = _value;
}
