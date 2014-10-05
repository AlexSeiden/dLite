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
};
#endif // REMAP_H
