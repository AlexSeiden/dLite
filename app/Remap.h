#ifndef REMAP_H
#define REMAP_H
#include "Node.h"

class Remap : public Node
{
public:
    Remap();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void    operator() ();
    virtual Remap* clone();

private:
    // Parameters
    Param<float> _output;
    Param<float> _minOut;
    Param<float> _maxOut;
    Param<float> _minIn;
    Param<float> _maxIn;
    Param<float> _input;

};
#endif // REMAP_H
