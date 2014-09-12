#ifndef BEATFILES_H
#define BEATFILES_H

#include "Node.h"
#include <vector>
#include <string>

// ------------------------------------------------------------------------------
//  TriggerEvery
class TriggerEvery : public Node
{
public:
    TriggerEvery();

    void setTriggerInterval(int interval) {_interval.setValue(interval);}
    void operator() ();

private:
    void    reset() {_output.setValue(false);}

    Param<bool>    _output;

    // These are in milliseconds
    // Parameters
    Param<int>     _interval;
    //Param<int>     _refreshOffset; // TODO

    // Internal timekeepers, in milliseconds
    int     _nextRefresh;

};


// ------------------------------------------------------------------------------
//  NodeOnset

// Imports "onset" data from sonic-annotator
// Onset data is generated by the "aubio-onset" plugin,
// and is a csv file with one datum per line: the sample ID
// of the next "onset"
class NodeOnset : public Node
{
public:
    NodeOnset();
    void operator() (void);

    int     getNextBeat(int sample);

    void    loadOnsetFile();
    void    loadOnsetFile(QString filename);

    Param<bool> _output;
    Param<int>  _offset; // TODO

    // May want to define a "file" param for input file.
    // Could be auto-filled to look in standard locations.
private:
    std::vector<int> _onsets;
    int _nextRefresh;
    int _nextIndex;
};

// ------------------------------------------------------------------------------
//  NodeBar

// Imports bar data from sonic-annotator
// This s generated by the "qm barbeat tracker" plugin,
// and is a csv file with two data per line: the sample ID
// of the next beat, and its bar number.  Since the bar numbers
// start at 1 and increase by one every sample, we don't explicitly
// record them here.
class NodeBar : public Node
{
public:
    NodeBar();
    void    operator() (void);

    void    loadFile();
    void    loadFile(QString filename);

    Param<bool> _output;
    Param<int>  _offset; // TODO

private:
    std::vector<int> _bars;
    int _nextRefresh;
    int _nextIndex;
};

// ------------------------------------------------------------------------------
//  NodeBarBeat

// Imports bar/beat data from sonic-annotator
// This s generated by the "qm barbeat tracker" plugin,
// and is a csv file with two data per line: the sample ID
// of the next beat, and its bar number.  Since the bar numbers
// start at 1 and increase by one every sample, we don't explicitly
// record them here.
class NodeBarBeat : public Node
{
public:
    NodeBarBeat();
    void    operator() (void);

    void    loadFile();
    void    loadFile(QString filename);

    Param<bool> _barTriggerOutput;
    Param<int>  _barNumberOutput;
    Param<bool> _beatTriggerOutput;
    Param<int>  _beatNumberOutput;
    Param<int>  _offset; // TODO

private:
    std::vector<int> _beats;
    std::vector<int> _beatnumber;
    int _nextRefresh;
    int _nextIndex;
};

// Free function proto
void convertSamplesToMS(std::vector<int> &samples);

#endif // BEATFILES_H
