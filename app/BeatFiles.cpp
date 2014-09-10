#include "BeatFiles.h"
#include "Node.h"
#include "Cupid.h"
#include <iostream>
#include <fstream>
#include <sstream>


// TODO master todo here
// (because it's the first src file.)
//
// Saving!
// Node Types:
    // color nodes:
    //      palette
    //      spline
    // beats!
    //      flesh out trigger node type
    //      view widgets
    //      segmentino
    //      breakdown viewer
    // Regions
    // Position
    //      Paths
// playback controller
//      progress bar with clicking
// Hardware interface!
// multiple tabbed cue sheets

// Cleaning:
//      Remnants of original spectrum audio recording stuff in buffer length and
//      window changed things


// ------------------------------------------------------------------------------
//  TriggerEvery
//      Triggers every _interval milliseconds

TriggerEvery::TriggerEvery() :
    Node(),
    _output(true),
    _interval(500)
{
    setName("TriggerEvery");
    _type = BEAT;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _interval.setName("interval");
    _interval.setOutput(false);
    _interval.setConnectable(false);

    // Set refresh to current time, so when we make a new one in the middle
    // of playing, it won't have to catch up.
    _nextRefresh = Cupid::getCurrentTime();
    _lastRefresh = Cupid::getCurrentTime();
    _paramList << &_output << &_interval;
    setParamParent();
}

void TriggerEvery::operator()()
{
    // Only run any operator once per frame:
    if (evaluatedThisFrame())
        return;

    // Automatically reset after triggering.
    _output._value = false;

    // XXX this breaks if we move the audio playhead back in time.
    qint64 mSecs =  Cupid::getCurrentTime() / 1000;
    if (mSecs > _nextRefresh) {
       _output._value = true;
       _lastRefresh = _nextRefresh;
       _nextRefresh += _interval._value;
    }
    _output._qvOutput.setValue(_output._value);
}


// ------------------------------------------------------------------------------
// NodeOnset
//      Loads "onset" info from precomputed file.
//      This node will generate an impulse (bool 'true')
//      for one frame after each onset.

NodeOnset::NodeOnset() : Node()
{
    setName("NodeOnset");
    _type = BEAT;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _offset.setName("offset");
    _offset.setOutput(false);
    _offset.setConnectable(false);

    _paramList << &_output << &_offset;
    setParamParent();
}

void NodeOnset::loadFile(std::string filename)
{
    std::ifstream filestream;
    filestream.open(filename, std::ios::in);
    if (! filestream.is_open())
        return; // ErrorHandling

    std::string line;
    while ( getline (filestream,line) ) {
        int value = std::atoi(line.c_str());
        _onsets.push_back(value);
    }
    filestream.close();
}

void NodeOnset::operator()()
{
    // XXX TODO:  ALL OPERATOR()s need to only exec once per 'frame'!

    // XXX implement operator
    _output._value = true;
}

// ------------------------------------------------------------------------------
// NodeBar
//      Loads bar (measure) info from precomputed file.

NodeBar::NodeBar() : Node()
{ }

void NodeBar::loadFile(std::string filename)
{
    std::ifstream filestream;
    filestream.open(filename, std::ios::in);
    if (! filestream.is_open())
        return; // ErrorHandling

    std::string line;
    while ( getline (filestream,line) ) {
        // Split at comma
        std::istringstream ss(line);
        std::string token;
        getline(ss, token, ',');

        int value = std::atoi(token.c_str());
        _bars.push_back(value);
    }
    filestream.close();
}

void NodeBar::operator()() {
    // XXX TODO:  ALL OPERATOR()s need to only exec once per 'frame'!

    // XXX implement operator
//    _output._value = true;
}


// ------------------------------------------------------------------------------
// Register
static Registrar<TriggerEvery>  registrar0("TriggerEvery", Node::BEAT);
static Registrar<NodeOnset>     registrar1("Onset", Node::BEAT);
static Registrar<NodeBar>       registrar2("Bar", Node::BEAT);
