#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "Node.h"
#include "Firing.h"

// Need forward class declaration, since both Cue.h and DanceFloor.h include each other
class Dancefloor;

class Cue : public Node
{
public:
    Cue();
    ~Cue();

    void            setCompMode(compMode_t mode)   {_compMode = mode;}
    compMode_t      getCompMode()                  {return _compMode;}
    void            setDecayMode(decayMode_t mode) {_decayMode = mode;}
    decayMode_t     getDecayMode()                 {return _decayMode;}

    virtual void    evaluate() = 0;
    virtual Cue*    clone() = 0;

    static void     setDancefloor(Dancefloor *dfmodel) {_dfModel = dfmodel;}

    void writeToJSONObj(QJsonObject &json) const;
    void readFromJSONObj(const QJsonObject &json);

protected:
    compMode_t      _compMode;      // TODO allow selection & implement compmodes
    decayMode_t     _decayMode;     // TODO allow selection of decaymodes

    // Having this be static means we don't have to set it for every cue,
    // but obviously limits us to one dfmodel.  This probably isn't an issue;
    // But having to init every cue with the dfmodel would have made the
    // instatiation code in CueLibView.cpp and/or Node.cpp more complicated,
    // since it would require passing an argument around.
    static Dancefloor *_dfModel;
};

#endif // CUE_H
