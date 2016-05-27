#include "PathNode.h"
#include "utils.h"
#include "Cupid.h"
#include "NodeFactory.h"
#include <QJsonObject>
#include <QJsonArray>

// Like SublevelNode, Path node's outputs aren't based on Params,
// but on a special editor.
PathNode::PathNode(QObject *parent) :
    QObject(parent)
{
    setName(QString("Path %1").arg(nodeCount()));
    m_type = POSITION;

    m_x.setName("x out");
    m_x.setConnectable(true);
    m_x.setOutput(true);

    m_y.setName("y out");
    m_y.setConnectable(true);
    m_y.setOutput(true);

    m_paramList << &m_x << &m_y;
}

void PathNode::operator()()
{
    if (evaluatedThisFrame())
        return;
    // Don't need to evalAllInputs() here because we know there are none!

    m_x._qvOutput = m_x.m_value;
    m_y._qvOutput = m_y.m_value;
}

PathNode* PathNode::clone()
{
    PathNode* lhs = new PathNode;
    cloneHelper(*lhs);
    return lhs;
}

void PathNode::beenSelected()
{
    // Tells the path editor to display the current subrange.
    // Handled implicitly now, but might be better to be explicit, e.g.:
    // emit iveBeenSelected(this);
    // I should build an better encapsulation of currently selected nodes
    // that use special editors.
    // e.g.
    //  the current sublevelnode for the spectrograph,
    //  the current path editor or region editor for the dance floor
    qDebug() << "path been selected";
}

void PathNode::beenDeselected()
{
    // See notes above.
    qDebug() << "path been deselected";
}

void PathNode::writeToJSONObj(QJsonObject &json) const
{
    Node::writeToJSONObj(json);

    // Override for serialization
    QJsonArray pathJsonArray;
    foreach (const Position pos, m_positions) {
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
        m_positions.push_back(pos);
    }
}


static Registrar<PathNode>   registrar("Path", Node::POSITION);
