#ifndef REGIONCUE_H
#define REGIONCUE_H

#include "nodes/RenderNode.h"
#include "engine/region.h"

class RegionCue : public RenderNode
{
public:
    RegionCue();
    void evaluate();

    void operator() (void);
    virtual RegionCue* clone();
};

#endif // REGIONCUE_H
