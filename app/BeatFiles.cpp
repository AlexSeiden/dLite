#include "BeatFiles.h"
#include "Node.h"
#include <iostream>
#include <fstream>
#include <sstream>


// TODO MASTER TODO HERE
// (because it's the first src file.)
//
// Connection semantics
//      output connections
//      non-anamatable params?
//      distinguish sources and sinks in drag & drop
//      distinguish sources and sinks icons
//      Graph view?
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
// Hardware interface!
// Need way of recalling instantiated nodes that have had their views closed


// ------------------------------------------------------------------------------
// NodeOnset
//      Loads "onset" info from precomputed file.

NodeOnset::NodeOnset() : Node()
{
    setName("NodeOnset");
    _type = BEAT;

    _output.setName("out");
    _output.setOutput(true);
    _output.setConnectable(true);

    _offset.setName("offset");
    _offset.setOutput(false);
    _offset.setConnectable(true);

    _paramList << &_output << &_offset;
}

void NodeOnset::loadFile(std::string filename)
{
    std::ifstream filestream;
    filestream.open(filename, std::ios::in);
    if (! filestream.is_open())
        return; // TODO error

    std::string line;
    while ( getline (filestream,line) ) {
        int value = std::atoi(line.c_str());
        _onsets.push_back(value);
    }
    filestream.close();
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
        return; // TODO error

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

static Registrar<NodeOnset>     registrar("Onset", Node::BEAT);
static Registrar<NodeBar>       registrar2("Bar", Node::BEAT);
