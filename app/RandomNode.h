#ifndef RANDOMNODE_H
#define RANDOMNODE_H
#include "Node.h"
#include <random>

class RandomFloat : public Node
{
public:
    RandomFloat();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() ();

//private:  // XXX

    // Parameters
    Param<float> _output;
    Param<float> _min;
    Param<float> _max;
//    TriggerEvery _trigger;

private:
    void  setRandomEngine();

    // Random number generator
    std::mt19937 *_randGenerator;
    std::uniform_real_distribution<float> *_distribution;
};

class RandomInt : public Node
{
public:
    RandomInt();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() ();

    virtual void paramHasBeenEdited();

//private:  // XXX
    int   _value;

    // Parameters
    Param<int> _output;
    Param<int> _min;
    Param<int> _max;
//    TriggerEvery _trigger;

private:
    void  setRandomEngine();

    // Random number generator
    std::mt19937 *_randGenerator;
    std::uniform_int_distribution<int> *_distribution;
};

class TriggerEvery : public Node
{

public:
    TriggerEvery() {}

    void setTriggerInterval(int interval) {_interval.setValue(interval);}
    void operator() ();

private:
    void    reset() {_output.setValue(false);}

    Param<bool>    _output;

    // These are in milliseconds
    // Parameters
    Param<int>     _interval;
    //Param<int>     _refreshOffset; // TODO

    // Internal timekeepers
    int     _lastRefresh;
    int     _nextRefresh;
};
#endif // RANDOMNODE_H
