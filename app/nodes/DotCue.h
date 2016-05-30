#ifndef DOTCUE_H
#define DOTCUE_H

#include "nodes/RenderNode.h"

class DotCue : public RenderNode
{
public:
    DotCue();
    void evaluate();

    void operator() (void);
    virtual DotCue* clone();
};

#endif // DOTCUE_H
