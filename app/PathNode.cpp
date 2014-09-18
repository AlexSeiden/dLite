#include "PathNode.h"
#include "utils.h"
#include "Cupid.h"
#include <QJsonObject>
#include <QJsonArray>

// Like SublevelNode, Path node's outputs aren't based on Params,
// but on a special editor.
PathNode::PathNode(QObject *parent) :
    QObject(parent)
{
    setName(QString("Path %1").arg(_nodeCount));
    _type = POSITION;

    _x.setName("x out");
    _x.setConnectable(true);
    _x.setOutput(true);

    _y.setName("y out");
    _y.setConnectable(true);
    _y.setOutput(true);

    _paramList << &_x << &_y;
    setParamParent();
}

void PathNode::operator()()
{
    if (evaluatedThisFrame())
        return;
    // Don't need to evalAllInputs() here because we know there are none!

    _x._qvOutput = _x._value;
    _y._qvOutput = _y._value;
}

void PathNode::beenSelected()
{
    // Tells the path editor to display the current subrange.
//    TODO emit iveBeenSelected(this);
    // Do we need the notion of currently selected nodes for editors of various kinds?
    // e.g.
    //  a current sublevelnode for the spectrograph,
    //  a current path editor or region editor for the dance floor
}

void PathNode::writeToJSONObj(QJsonObject &json) const
{
    Node::writeToJSONObj(json);

    // Override for serialization
    QJsonArray pathJsonArray;
    foreach (const Position pos, _positions) {
         QJsonObject posJ;
         posJ["x"] = pos.x;
         posJ["y"] = pos.y;
         pathJsonArray.append(posJ);
     }
    json["positions"] = pathJsonArray;
}

void PathNode::readFromJSONObj(const QJsonObject &json)
{
    // Override for serialization
    Node::readFromJSONObj(json);
    QJsonArray positionsArray = json["positions"].toArray();
    for (int i=0; i<positionsArray.size(); ++i) {
        QJsonObject posJsonObject = positionsArray[i].toObject();
        Position pos;
        pos.x = posJsonObject["x"].toInt();
        pos.y = posJsonObject["y"].toInt();
        _positions.push_back(pos);      // (Hopefully) this uses copy to push onto QList of positons.
    }
}


static Registrar<PathNode>   registrar("PathNode", Node::POSITION);
