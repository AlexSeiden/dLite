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

protected:
    Param<Lightcolor>  _color;
    Param<float>       _alpha;
    Param<Region>      _region;
};

#endif // REGIONCUE_H
