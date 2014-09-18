#include "RegionNode.h"
#include "utils.h"
#include "Cupid.h"
#include <QJsonObject>
#include <QJsonArray>

// Like SublevelNode, Region node's outputs aren't based on Params,
// but on a special editor.
RegionNode::RegionNode(QObject *parent) :
    QObject(parent)
{
    setName(QString("Region %1").arg(_nodeCount));
#if 0
    _type = POSITION;

    _x.setName("x out");
    _x.setConnectable(true);
    _x.setOutput(true);

    _y.setName("y out");
    _y.setConnectable(true);
    _y.setOutput(true);

    _paramList << &_x << &_y;
    setParamParent();
#endif
}

void RegionNode::operator()()
{
#if 0
    if (evaluatedThisFrame())
        return;
    // Don't need to evalAllInputs() here because we know there are none!

    _x._qvOutput = _x._value;
    _y._qvOutput = _y._value;
#endif
}

void RegionNode::beenSelected()
{
#if 0
    // Tells the Region editor to display the current subrange.
//    TODO emit iveBeenSelected(this);
    // Do we need the notion of currently selected nodes for editors of various kinds?
    // e.g.
    //  a current sublevelnode for the spectrograph,
    //  a current path editor or region editor for the dance floor
#endif
}

void RegionNode::writeToJSONObj(QJsonObject &json) const
{
    Node::writeToJSONObj(json);

#if 0
    // Override for serialization
    QJsonArray regionJsonArray;
    foreach (const Position pos, _positions) {
         QJsonObject posJ;
         posJ["x"] = pos.x;
         posJ["y"] = pos.y;
         regionJsonArray.append(posJ);
     }
    json["positions"] = regionJsonArray;
#endif
}

void RegionNode::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
#if 0
    QJsonArray positionsArray = json["positions"].toArray();
    for (int i=0; i<positionsArray.size(); ++i) {
        QJsonObject posJsonObject = positionsArray[i].toObject();
        Position pos;
        pos.x = posJsonObject["x"].toInt();
        pos.y = posJsonObject["y"].toInt();
        _positions.push_back(pos);      // (Hopefully) this uses copy to push onto QList of positons.
    }
#endif
}


static Registrar<RegionNode>   registrar("RegionNode", Node::POSITION);
