#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "dancefloormodel.h"
#include "Param.h"

class Cue
{
public:
    explicit Cue(Dancefloormodel *dfmodel = 0);
    void evaluate();

    void            setCompMode(compmode_t mode);
    compmode_t      getCompMode();

private:
    Dancefloormodel *_floor;
    bool            _active;
    compmode_t      _compmode;

public:  // TODO just for testing
    Param<int>      _xoffset;
    Param<int>      _yoffset;
    Param<float>    _scale;
    Param<float>    _alpha;
    Lightcolor _color;
};


#endif // CUE_H
