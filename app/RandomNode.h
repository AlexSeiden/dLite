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

    void paramHasBeenEdited();

private:
    // Parameters
    Param<float> _output;
    Param<float> _min;
    Param<float> _max;
    Param<bool> _trigger;

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
    void operator() ();
    void paramHasBeenEdited();

private:
    // Parameters
    Param<int> _output;
    Param<int> _min;
    Param<int> _max;
    Param<bool> _trigger;

private:
    void  setRandomEngine();

    // Random number generator
    std::mt19937 *_randGenerator;
    std::uniform_int_distribution<int> *_distribution;
};

class SequenceInt : public Node
{
public:
    SequenceInt();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() ();

    void paramHasBeenEdited();

private:
    // Parameters
    Param<int> _output;
    Param<int> _min;
    Param<int> _max;
    Param<bool> _trigger;

private:
    int _counter;
};

#endif // RANDOMNODE_H
