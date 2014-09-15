#include "Cue.h"
#include "dancefloormodel.h"

// ------------------------------------------------------------------------------
//  Cue
//  Abstract base class

Cue::Cue() :
    _compmode(SET)
{
    _type = CUE;
    // Add this to list of cues the model knows about.
    if (Cue::_dfModel)
        Cue::_dfModel->addCue(this);
}
