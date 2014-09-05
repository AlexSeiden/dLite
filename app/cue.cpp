#include "Cue.h"
#include "dancefloormodel.h"

void randLight(int &value) // XXX
{
    value = rand() % 18;
}

// ------------------------------------------------------------------------------
//  Cue
//  Abstract base class

Cue::Cue() :
    Node(),
    _compmode(SET)
{
    // Add this to list of cues the model knows about.
    if (Cue::_dfModel)
        Cue::_dfModel->addCue(this);
}
