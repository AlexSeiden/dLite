#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "dancefloormodel.h"
#include "Node.h"

// Need forward class declaration, since both Cue.h and dancefloormodel.h include each other
class Dancefloormodel;

class Cue : public Node
{
public:
    Cue(Dancefloormodel *dfmodel);

    void            setCompMode(compmode_t mode);
    compmode_t      getCompMode();

    virtual void    evaluate() = 0;

protected:
    Dancefloormodel *_dfModel;
    compmode_t      _compmode;      // TODO implement compmodes
    int             _decaymode;     // TODO allow selection of decaymodes
};

#endif // CUE_H
