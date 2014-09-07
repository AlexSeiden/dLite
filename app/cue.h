#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "Node.h"

// Need forward class declaration, since both Cue.h and dancefloormodel.h include each other
class Dancefloormodel;

class Cue : public Node
{
public:
    Cue();

    void            setCompMode(compmode_t mode);
    compmode_t      getCompMode();

    static void     setDancefloormodel(Dancefloormodel *dfmodel) {_dfModel = dfmodel;}


    virtual void    evaluate() = 0;

protected:
    // Having this be static means we don't have to set it for every cue,
    // but obviously limits us to one dfmodel.  This probably isn't an issue;
    // But having to init every cue with the dfmodel would have made the
    // instatiation code in CueLibView.cpp and/or Node.cpp more complicated,
    // since it would require passing an argument around.
    static Dancefloormodel *_dfModel;
    compmode_t      _compmode;      // TODO implement compmodes
    int             _decaymode;     // TODO allow selection of decaymodes
};

#endif // CUE_H
