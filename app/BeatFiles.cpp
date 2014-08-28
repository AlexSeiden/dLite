#include "BeatFiles.h"
#include <iostream>
#include <fstream>
#include <sstream>

NodeOnset::NodeOnset() { }

void
NodeOnset::loadFile(std::string filename)
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

NodeBar::NodeBar() { }

void
NodeBar::loadFile(std::string filename)
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
