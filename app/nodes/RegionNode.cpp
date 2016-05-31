#include "nodes/RegionNode.h"
#include "engine/nodefactory.h"
#include "engine/utils.h"
#include "engine/dispatch.h"
#include <QJsonObject>
#include <QJsonArray>
#include <functional>
#include "views/dance_floor_widget.h"

// Like SublevelNode, Region node's outputs aren't based on Params,
// but on a special editor.
RegionNode::RegionNode(QObject *parent) :
    QObject(parent)
{
    setName(QString("Region%1").arg(nodeCount()));
    m_type = REGION;

    Param<Region>* prp = addParam<Region>("out", Region(), true);
    m_regionP = &(prp->m_value);

    CHECKED_CONNECT(this,
                    SIGNAL(regionNodeSelected(RegionNode*)),
                    Dispatch::Singleton()->getDancefloorwidget(),
                    SLOT(regionSelected(RegionNode*)));

    CHECKED_CONNECT(this,
                    SIGNAL(regionNodeDeselected(RegionNode*)),
                    Dispatch::Singleton()->getDancefloorwidget(),
                    SLOT(regionDeselected(RegionNode*)));
}

RegionNode::~RegionNode()
{
    beenDeselected();
}

void RegionNode::operator()()
{
    if (evaluatedThisFrame())
        return;

    Region region;
    getValue("out", region);
    // Need to do this to make sure qvOutput gets correct value
    setValue("out", region);
}

RegionNode* RegionNode::clone()
{
    RegionNode* lhs = new RegionNode;
    cloneHelper(*lhs);
    return lhs;
}

void RegionNode::beenSelected()
{
    emit regionNodeSelected(this);
}

void RegionNode::beenDeselected()
{
    emit regionNodeDeselected(this);
}

// Serialization
void RegionNode::writeToJSONObj(QJsonObject &json) const
{
    Node::writeToJSONObj(json);

    // Override for serialization
    m_regionP->writeToJSONObj(json);
}

void RegionNode::readFromJSONObj(const QJsonObject &json)
{
    Node::readFromJSONObj(json);
    m_regionP->readFromJSONObj(json);
}

bool RegionNode::hasCell(QPoint p)
{
    return m_regionP->hasCell(p);
}

void RegionNode::setCell(QPoint p, bool status)
{
    m_regionP->setCell(p, status);
}

PixelQueryDelegate_t RegionNode::getQueryDelegate()
{
    auto out = [&] (QPoint p) {
        return (this->hasCell(p) ? QColor(255,255,255): QColor(0,0,0));
    } ;
    return out;
}

PixelEditDelegate_t RegionNode::getEditDelegate()
{
    auto out = [&] (QPoint p, bool status) { return (this->setCell(p,status));};
    return out;
}

static Registrar<RegionNode>   registrar("Region", Node::REGION);
