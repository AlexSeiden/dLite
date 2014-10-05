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
};

#endif // DOTCUE_H
