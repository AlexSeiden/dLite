#include "RegionNode.h"
#include "utils.h"
#include "Cupid.h"
#include <QJsonObject>
#include <QJsonArray>
#include <functional>
#include "dancefloorwidget.h"

// Like SublevelNode, Region node's outputs aren't based on Params,
// but on a special editor.
RegionNode::RegionNode(QObject *parent) :
    QObject(parent)
{
    setName(QString("Region%1").arg(nodeCount()));
    _type = REGION;

    _region.setName("out");
    _region.setConnectable(true);
    _region.setOutput(true);

    _paramList << &_region;
    setParamParent();

    CHECKED_CONNECT(this,
                    SIGNAL(regionNodeSelected(RegionNode*)),
                    Cupid::Singleton()->getDancefloorwidget(),
                    SLOT(regionSelected(RegionNode*)));

    CHECKED_CONNECT(this,
                    SIGNAL(regionNodeDeselected(RegionNode*)),
                    Cupid::Singleton()->getDancefloorwidget(),
                    SLOT(regionDeselected(RegionNode*)));
}

void RegionNode::operator()()
{
    if (evaluatedThisFrame())
        return;

    _region._qvOutput.setValue(_region._value);
}

RegionNode* RegionNode::clone()
{
    RegionNode* lhs = new RegionNode;
    cloneHelper(*lhs);
    setParamParent();
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
    _region._value.writeToJSONObj(json);
}

void RegionNode::readFromJSONObj(const QJsonObject &json)
{
    Node::readFromJSONObj(json);
    _region._value.readFromJSONObj(json);
}

bool RegionNode::hasCell(QPoint p)
{
    return _region._value.hasCell(p);
}

void RegionNode::setCell(QPoint p, bool status)
{
    _region._value.setCell(p, status);
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
