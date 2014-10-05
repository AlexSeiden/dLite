#ifndef REGIONCUE_H
#define REGIONCUE_H

#include "Cue.h"
#include "Region.h"

class RegionCue : public Cue
{
public:
    RegionCue();
    void evaluate();

    void operator() (void);
    virtual RegionCue* clone();
};

#endif // REGIONCUE_H
