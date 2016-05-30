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
    Firing(Lightcolor color, double alpha, compMode_t compmode,
           decayfunc_t decayfunc, RenderNode *cue=nullptr);

    bool        evaluate();

    Lightcolor  compOver(const Lightcolor &lightcolor);
    Lightcolor  compAdd(const Lightcolor &lightcolor);
    Lightcolor  compSet(const Lightcolor &lightcolor);

    void        setCompMode(compMode_t cm) {m_compMode = cm;}
    void        setDecayMode(decayMode_t dmode);

    Lightcolor  m_color;
    float       m_alpha;
    compMode_t  m_compMode;
    decayfunc_t m_decayfunction;
    RenderNode *       m_cue;      // mainly used when the same cue wants to replace
                            // a firing in the buffer, rather than add to it.
};


#endif // FIRING_H
