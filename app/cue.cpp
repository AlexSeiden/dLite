#include "Cue.h"
#include "DanceFloor.h"

// ------------------------------------------------------------------------------
//  Cue
//  Abstract base class

Cue::Cue() :
    _compMode(SET),
    _decayMode(IMMEDIATE)
{
    _type = CUE;
    // Add this to list of cues the model knows about.
    Q_ASSERT(Cue::_dfModel);
    Cue::_dfModel->addCue(this);
}

void Cue::write(QJsonObject &json) const
{
    // Override for serialization
    Node::write(json);
    json["compMode"] = _compMode;       // XXX
    json["decaymode"] = _decayMode;     // XXX
}

void Cue::read(const QJsonObject &json)
{
    // Override for serialization
    Node::read(json);
    _compMode = compMode_t(qRound(json["compMode"].toDouble()));   // XXX
    _decayMode = decayMode_t(qRound(json["decayMode"].toDouble()));   // XXX
}
