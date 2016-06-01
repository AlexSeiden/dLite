// Copyright (C) 2014-2016 Alex Seiden
//
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
