#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "dancefloormodel.h"
#include "Param.h"

enum compmode_t {SET, ADD, OVER, UNDER};

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

    ParamInt      _xoffset;
    ParamInt      _yoffset;
    ParamFloat    _scale;
    ParamFloat    _alpha;
    Lightcolor _color;
};


#endif // CUE_H
