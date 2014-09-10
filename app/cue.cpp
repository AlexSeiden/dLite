#include "Cue.h"
#include "dancefloormodel.h"

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
