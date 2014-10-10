#include "Node.h"
#include "utils.h"
#include "Cupid.h"
#include <iostream>
#include <string>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>
#include <QRegularExpression>
#include "GraphWidget.h"        // GROSS

// ------------------------------------------------------------------------------
//  Node

QList<Node*> Node::_allNodes;

Node::Node() :
    _name(QString()),
    _type(UNDEFINED),
    _frameLastEvaluated(-1),
    _uuid(QUuid::createUuid()),
    _nodeItem(nullptr)
{
    // Style Hmmm, this might be a good place to use a weak ptr.
    _allNodes.append(this);

    addParam<bool>("active", true, false, false);
    // Could potentially be connectable,
    // but would have to make sure
    // that connection was evaluated
    // prior to use.
}

Node::~Node() {
    _allNodes.removeAll(this);
}

bool Node::isActive() const
{
    bool active;
    getValue("active", active);
    return active;
}

void Node::setActive(bool status)
{
    setValue("active", status);
}

void Node::cloneHelper(Node& lhs)
{
    // Things that are simply copied
    // (Although these first two are set from the regular ctor,
    // and shouldn't change.)
    lhs._type       = _type;
    lhs._className  = _className;

    // Things that are modified
    lhs._name     = Node::uniqueName(_name);
    lhs._frameLastEvaluated = -1;
    lhs._uuid     = QUuid::createUuid();

    // Copy parameter values and connections.
    // If connections are within a group that is being
    // copied together, the caller (e.g. NodeFactory::duplicate)
    // handles making the appropriate reconnections.
    foreach (ParamBase* lhsParam, lhs._paramList) {
        ParamBase* rhsParam = getParamByName(lhsParam->getName());
        lhsParam->copyValueAndConnection(rhsParam);
    }
}

/*
The beenSelected() and beenDeselected() methods are virtual
methods that are overridden in nodes like SublevelNode and
Region, which will put other windows (e.g. the spectrograph
and Dancefloorwidget, respectively) that they should be
in their special editing mode.
*/
void Node::beenSelected() {}
void Node::beenDeselected() {}

/*
loadFile methods are virtual methods that are overridden in
nodes like BarBeat, etc., which read precomputed data from a
file on disk.  It's a virtual method of Node so that when the
song changes, it's easy to just call loadFile() on all nodes
and the nodes that need to reload will do so.
*/
void Node::loadFile() {}
void Node::loadFile(QString filename) {Q_UNUSED(filename)}

bool Node::nameIsUnique(QString name)
{
    foreach (Node* node, _allNodes) {
        if (name == node->getName())
            return false;
    }
    return true;
}

QString Node::uniqueName(QString name)
{
    if (nameIsUnique(name))
        return name;

    QRegularExpression re("(.*)(\\d{1,3})$");
    QRegularExpressionMatch match = re.match(name);
    QString newNameCandidate;
    if (match.hasMatch()) {
        QString prefix = match.captured(1);
        QString suffix = match.captured(2);
        int index = suffix.toInt();
        index++;
        newNameCandidate = prefix + QString::number(index);
    }
    else {
        newNameCandidate = name + QString::number(1);
    }
    return uniqueName(newNameCandidate);
}

Node::node_t Node::getType()
{
    return _type;
}

void Node::evalAllInputs()
{
    // Evaluates the providers connected to any input parameters.
    foreach (ParamBase *p, getParams()) {
        if (! p->isOutput()) {
            p->eval();
        }
    }
}

bool Node::evaluatedThisFrame()
{
    /*
    Predicate that returns true if the node has already been evaluated
    on this frame, and therefore shouldn't be evaluted again.
    */
    int frame =  Cupid::getCurrentFrame();
    if (frame == _frameLastEvaluated)
        return true;

    _frameLastEvaluated = frame;
    return false;
}

// ------------------------------------------------------------------------------
// Serialization
void Node::readFromJSONObj(const QJsonObject &json)
{
    setName(json["name"].toString());
    // Redundant because active is just another parameter, and is handled as part of
    // the paramlist.
//    setActive(json["active"].toBool());

}

void Node::writeToJSONObj(QJsonObject &json) const
{
    // ErrorHandling

    json["name"] = getName();
    // by "classname", I mean the name used as used in the NodeFactory registry.
    json["classname"] = _className;
    json["uuid"] = _uuid.toString();

    QJsonArray paramJsonArray;
    foreach (const ParamBase *param, _paramList) {
         QJsonObject paramJ;
         param->writeToJSONObj(paramJ);
         paramJsonArray.append(paramJ);
     }
    json["paramList"] = paramJsonArray;

     // write graph view info
    Cupid::Singleton()->getGraphWidget()->writeNodeUiToJSONObj(this, json);
}

// getParamByName()
ParamBase *Node::getParamByName(QString paramname)
{
/* Utility function that's used when reading a file from disk,
 * and when matching parameters during duplication.
 */
    if (_paramDict.contains(paramname))
        return _paramDict[paramname];
    return nullptr;
    // ErrorHandling
}
