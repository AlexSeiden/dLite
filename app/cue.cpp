#include "Cue.h"
#include "DanceFloor.h"

// ------------------------------------------------------------------------------
//  Cue
//  Abstract base class

Dancefloor *Cue::_dfModel = nullptr;

Cue::Cue() :
    _compMode(ADD),
    _decayMode(IMMEDIATE)
{
    _type = CUE;
    // Add this to list of cues the model knows about.
    Q_ASSERT(Cue::_dfModel);
    Cue::_dfModel->addCue(this);
}

Cue::~Cue()
{
    Cue::_dfModel->removeCue(this);
    // This is removed from node inventory in Node dtor.
}

void Cue::writeToJSONObj(QJsonObject &json) const
{
    // Override for serialization
    Node::writeToJSONObj(json);
    json["compMode"] = _compMode;       // XXX
    json["decaymode"] = _decayMode;     // XXX
}

void Cue::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
    _compMode = compMode_t(qRound(json["compMode"].toDouble()));   // XXX
    _decayMode = decayMode_t(qRound(json["decayMode"].toDouble()));   // XXX
}
