#ifndef FIRING_H
#define FIRING_H

#include "lightcolor.h"

// -----------------------------------------------------------------------------
// Firing

class Firing;
typedef bool (*decayfunc_t)(Firing *);

class Firing
{
public:
    Firing();
    Firing(Lightcolor color, double alpha, compMode_t compmode, decayfunc_t decayfunc, Cue *cue=nullptr);

    bool        evaluate();

    Lightcolor  compOver(const Lightcolor &lightcolor);
    Lightcolor  compAdd(const Lightcolor &lightcolor);
    Lightcolor  compSet(const Lightcolor &lightcolor);

    void        setCompMode(compMode_t cm) {_compMode = cm;}
//    void        setDecayMode(decayfunc_t dfunk) {_decayfunction = dfunk;}
    void        setDecayMode(decayMode_t dmode);

    Lightcolor  _color;
    float       _alpha;
    compMode_t  _compMode;
    decayfunc_t _decayfunction;
    Cue         *_cue; // mainly used when the same cue wants to replace
                      // a firing in the buffer, rather than add to it.
};


#endif // FIRING_H
