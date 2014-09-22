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

    _out.setName("out");
    _out.setConnectable(true);
    _out.setOutput(true);

    _paramList << &_out;
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

    // Don't need to evalAllInputs() here because we know there are none!
#if 0

    _x._qvOutput = _x._value;
    _y._qvOutput = _y._value;
#endif
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
    QJsonArray regionJsonArray;
    foreach (const QPoint cell, _cells) {
         QJsonObject posJ;
         posJ["x"] = cell.x();
         posJ["y"] = cell.y();
         regionJsonArray.append(posJ);
     }
    json["cells"] = regionJsonArray;
}

void RegionNode::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
    QJsonArray positionsArray = json["cells"].toArray();
    for (int i=0; i<positionsArray.size(); ++i) {
        QJsonObject posJsonObject = positionsArray[i].toObject();
        int x = posJsonObject["x"].toInt();
        int y = posJsonObject["y"].toInt();
        _cells.push_back(QPoint(x,y));
        // (Hopefully) this uses copy to push onto QList of cells.
    }
}

bool RegionNode::hasCell(QPoint p)
{
    return _cells.contains(p);
}

void RegionNode::setCell(QPoint p, bool status)
{
    // $$$ inefficient!!
    _cells.removeAll(p);
    if (status)
        _cells.append(p);
}

PixelQueryDelegate_t RegionNode::getQueryDelegate()
{
// test delegate:
#if 0
    auto out = [] (QPoint p) {
        return ((p.x()+p.y())%2) ? QColor(255,255,255): QColor(0,0,0);
    } ;
#else

    auto out = [&] (QPoint p) {
        return (this->hasCell(p) ? QColor(255,255,255): QColor(0,0,0));
    } ;
#endif
    return out;
}

PixelEditDelegate_t RegionNode::getEditDelegate()
{
    auto out = [&] (QPoint p, bool status) { return (this->setCell(p,status));};
    return out;
}

static Registrar<RegionNode>   registrar("Region", Node::REGION);
