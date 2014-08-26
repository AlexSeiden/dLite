#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "dancefloormodel.h"
#include "Param.h"

class Dancefloormodel;


class Cue
{
public:
    explicit Cue(Dancefloormodel *dfmodel = 0);
    void evaluate();

    void            setCompMode(compmode_t mode);
    compmode_t      getCompMode();

    const QString & getName() const {return _name;}
    void            setName(const QString& name) {_name = name;}

private:
    Dancefloormodel *_dfModel;
    bool            _active;
    compmode_t      _compmode;      // TODO implement compmodes

public:  // TODO    just public for testing
    Param<int>      _xoffset;
    Param<int>      _yoffset;
    Param<float>    _scale;
    Param<float>    _alpha;
    Lightcolor      _color;
    QString         _name;
};


#endif // CUE_H
