#ifndef COLORNODES_H
#define COLORNODES_H
#include "Node.h"

class ColorRamp : public Node
{
public:
    ColorRamp();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() ();

private:
    // Parameters
    Param<Lightcolor> _output;
    Param<Lightcolor> _c0;
    Param<Lightcolor> _c1;
    Param<float> _mix;

private:

};


#endif // COLORNODES_H
