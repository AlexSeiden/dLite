#ifndef CUEBOX_H
#define CUEBOX_H

#include "Cue.h"

class CueBox : public Cue
{
public:
    explicit CueBox(Dancefloormodel *dfmodel = 0);
    void evaluate();

public:  // TODO    just public for testing
    Param<int>      _xoffset;
    Param<int>      _yoffset;
    Param<float>    _scale;
    Param<float>    _alpha;
    Param<Lightcolor>      _color;
};

#endif // CUEBOX_H
