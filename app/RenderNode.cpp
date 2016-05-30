#include "Cue.h"
#include "DanceFloor.h"
#include "Cupid.h"

// ------------------------------------------------------------------------------
//  Cue
//  Abstract base class for all Cue types
// (i.e. Nodes that write to the frame buffer)

Dancefloor *Cue::m_dfModel = nullptr;

Cue::Cue() :
    m_compMode(ADD),
    m_decayMode(IMMEDIATE)
{
    m_type = CUE;
    // Add this to list of cues the model knows about.
    Q_ASSERT(Cue::m_dfModel);
    Cue::m_dfModel->addCue(this);
    // TODO should add to cuesheet here rather than through GraphWidget::addNode
}

Cue::~Cue()
{
    Cue::m_dfModel->removeCue(this);
    // This is removed from node inventory via Node dtor.
}

void Cue::writeToJSONObj(QJsonObject &json) const
{
    // Override for serialization
    Node::writeToJSONObj(json);
    json["compMode"] = m_compMode;
    json["decaymode"] = m_decayMode;
}

void Cue::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
    m_compMode = compMode_t(qRound(json["compMode"].toDouble()));
    m_decayMode = decayMode_t(qRound(json["decayMode"].toDouble()));
}
