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
    void    operator() ();
    virtual RandomFloat* clone();
    void    paramHasBeenEdited();

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
    void    operator() ();
    virtual RandomInt* clone();
    void    paramHasBeenEdited();

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

    void    operator() ();
    virtual SequenceInt* clone();
    void    paramHasBeenEdited();

private:
    int _counter;
};

class IntToFloat : public Node
{
public:
    IntToFloat();

    void    operator() ();
    virtual IntToFloat* clone();
    void    paramHasBeenEdited();
};

#endif // RANDOMNODE_H
