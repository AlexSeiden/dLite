#ifndef CUEBOX_H
#define CUEBOX_H

#include "Cue.h"

class CueBox : public Cue
{
public:
    CueBox();
    void evaluate();

    void operator() (void);

    friend class CueBoxView;

protected:
    Param<int>      _x;
    Param<int>      _y;
    Param<float>    _scale;
    Param<float>    _alpha;
    Param<Lightcolor>      _color;
};

#endif // CUEBOX_H
