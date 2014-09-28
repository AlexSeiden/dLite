#ifndef DOTCUE_H
#define DOTCUE_H

#include "Cue.h"

class DotCue : public Cue
{
public:
    DotCue();
    void evaluate();

    void operator() (void);
    virtual DotCue* clone();

protected:
    Param<int>      _x;
    Param<int>      _y;
    Param<float>    _scale;
    Param<float>    _alpha;
    Param<Lightcolor>      _color;
};

#endif // DOTCUE_H
