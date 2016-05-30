// ------------------------------------------------------------------------------
//  RenderNode
//  Abstract base class for all nodes that write to the frame buffer.

#include "nodes/RenderNode.h"
#include "engine/dancefloor.h"
#include "engine/dispatch.h"

Dancefloor *RenderNode::m_dfModel = nullptr;

RenderNode::RenderNode() :
    m_compMode(ADD),
    m_decayMode(IMMEDIATE)
{
    m_type = CUE;
    // Add this to list of cues the model knows about.
    Q_ASSERT(RenderNode::m_dfModel);
    RenderNode::m_dfModel->addRenderNode(this);

    // TODO should add to cuesheet here rather than through GraphWidget::addNode
}

RenderNode::~RenderNode()
{
    RenderNode::m_dfModel->removeRenderNode(this);
    // This is removed from node inventory via Node dtor.
}

void RenderNode::writeToJSONObj(QJsonObject &json) const
{
    // Override for serialization
    Node::writeToJSONObj(json);
    json["compMode"] = m_compMode;
    json["decaymode"] = m_decayMode;
}

void RenderNode::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
    m_compMode = compMode_t(qRound(json["compMode"].toDouble()));
    m_decayMode = decayMode_t(qRound(json["decayMode"].toDouble()));
}
