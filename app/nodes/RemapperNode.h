#ifndef REMAPPER_NODE_H
#define REMAPPER_NODE_H
#include "nodes/Node.h"

class RemapperNode : public Node
{
public:
    RemapperNode();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void    operator() ();
    virtual RemapperNode* clone();
};
#endif // REMAPPER_NODE_H
