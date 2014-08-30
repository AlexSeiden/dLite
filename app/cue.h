#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "dancefloormodel.h"
#include "Param.h"

// Need forward class declaration, since both Cue.h and dancefloormodel.h include each other
class Dancefloormodel;

class Cue
{
public:
    Cue(Dancefloormodel *dfmodel);

    const QString & getName() const {return _name;}
    void            setName(const QString& name) {_name = name;}

    void            setCompMode(compmode_t mode);
    compmode_t      getCompMode();

    bool            isActive() {return _active;}
    void            setActive(bool active) {_active = active;}

    virtual void    evaluate() = 0;

protected:
    Dancefloormodel *_dfModel;
    bool            _active;
    compmode_t      _compmode;      // TODO implement compmodes
    int             _decaymode;     // TODO allow selection of decaymodes

    QString         _name;
};

#endif // CUE_H
