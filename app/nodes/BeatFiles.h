// Copyright (c) Alex Seiden 2014-2016
//
#ifndef BEATFILES_H
#define BEATFILES_H

#include "nodes/Node.h"
#include <vector>
#include <string>

// ------------------------------------------------------------------------------
//  TriggerEvery
class TriggerEvery : public Node
{
public:
    TriggerEvery();

    void operator() ();
    virtual TriggerEvery* clone();

private:
    void    reset() {setValue("out", false); m_nextRefresh=-1;}

    // Internal timekeeper, in milliseconds
    int     m_nextRefresh;
};


// ------------------------------------------------------------------------------
//  Multiply
class Multiply : public Node
{
public:
    Multiply();
    void operator() ();
    virtual Multiply* clone();

private:
    void    reset() {setValue("out", false); m_nextRefresh=-1;}

    // Internal timekeepers, in milliseconds
    int     m_nextRefresh;
    int     m_lastInputBeat;
    int     m_delta;
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
    virtual NodeOnset* clone();

    // TODO for random access:
    // bool     checkBeat(int thisSample, int lastSample);

    void    loadFile();
    void    loadFile(QString filename);

    // May want to define a "file" param for input file.
    // Could be auto-filled to look in standard locations.

private:
    std::vector<int>    m_onsets;
    int                 m_nextRefresh;
    int                 m_nextIndex;
    int                 m_lastSample;
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
    virtual NodeBar* clone();

    void    loadFile();
    void    loadFile(QString filename);

private:
    std::vector<int>    m_bars;
    int                 m_nextRefresh;
    int                 m_nextIndex;
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
    virtual NodeBarBeat* clone();

    void    loadFile();
    void    loadFile(QString filename);


private:
    int     findStartOfBar(int beatindex);
    int     findStartOfNextBar(int beatindex);

    std::vector<int>    m_beats;
    std::vector<int>    m_beatnumber;
    std::vector<int>    m_barnumber;
    int                 m_beatIndex;

    int                 m_lastSample;
};

// ------------------------------------------------------------------------------
//  Segmentino

class Segmentino : public Node
{
public:
    Segmentino();
    void    operator() (void);
    virtual Segmentino* clone();

    void    loadFile();
    void    loadFile(QString filename);

private:
    std::vector<std::pair<int, int> > _segments;
    int findSegment(int msecs);
};

// Free function proto
void convertSamplesToMS(std::vector<int> &samples);

#endif // BEATFILES_H
