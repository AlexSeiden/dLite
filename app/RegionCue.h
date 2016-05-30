#ifndef REGIONCUE_H
#define REGIONCUE_H

#include "RenderNode.h"
#include "Region.h"

class RegionCue : public RenderNode
{
public:
    RegionCue();
    void evaluate();

    void operator() (void);
    virtual RegionCue* clone();
};

#endif // REGIONCUE_H
