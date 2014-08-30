#include "Cue.h"
#include "dancefloormodel.h"

void randLight(int &value) // XXX
{
    value = rand() % 18;
}

// ------------------------------------------------------------------------------
//  Cue
//  Abstract base class

Cue::Cue(Dancefloormodel *dfmodel) :
    _dfModel(dfmodel),
    _active(true),
    _compmode(SET)
{
    // Add this to list of cues the model knows about.
    _dfModel->addCue(this);
}
