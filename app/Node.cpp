#include "Node.h"
#include "utils.h"
#include "Cupid.h"
#include <iostream>
#include <string>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>
#include "GraphWidget.h"        // GROSS

// ------------------------------------------------------------------------------
//  Node
//  base class

int Node::_nodeCount = 0;

Node::Node() :
    _name(QString()),
    _active(true),
    _type(UNDEFINED),
    _uuid(QUuid::createUuid())
{
    _nodeCount++;
}

Node::~Node() {}

// The "beenSelected" method is virtual so that nodes like Sublevels
// & Paths can communicate with special editors.
void Node::beenSelected() {}

Node::node_t Node::getType()
{
    return _type;
}


// GROSS.  at least, should be a better way to init this.
// should be a better way to find out without having to maintain this.
// This kinda breaks encapsulation to have it in the first place.
void Node::setParamParent()
{
    foreach (ParamBase *p, _paramList) {
        p->setParent(this);
    }
}

void Node::evalAllInputs()
{
    foreach (ParamBase *p, getParams()) {
        if (! p->isOutput()) {
            p->eval();
        }
    }
}

bool Node::evaluatedThisFrame()
{
    int frame =  Cupid::getCurrentFrame();
    if (frame == _frameLastEvaluated)
        return true;

    _frameLastEvaluated = frame;
    return false;
}

void Node::readFromJSONObj(const QJsonObject &json)
{
    _name      = json["name"].toString();
    _active    = json["active"].toBool();

}

void Node::writeToJSONObj(QJsonObject &json) const
{
    // ErrorHandling

    json["name"] = _name;
    json["active"] = _active;
    // "classname" here is the name used in the NodeFactory registry.
    json["classname"] = _className;
    json["uuid"] = _uuid.toString();
    //json["type"] = _type; // Type is unchanging per class.

    QJsonArray paramJsonArray;
    foreach (const ParamBase *param, _paramList) {
         QJsonObject paramJ;
         param->writeToJSONObj(paramJ);
         paramJsonArray.append(paramJ);
     }
    json["paramList"] = paramJsonArray;

     // TODO write graph view info
}

ParamBase *Node::getParamByName(QString paramname)
{
    // Return the parameter called "paramname"
    foreach (ParamBase *p, _paramList) {
        if (paramname == p->getName())
            return p;
    }
    // ErrorHandling
    return nullptr;
}

// ------------------------------------------------------------------------------
//  Node Factory

NodeFactory::NodeFactory() {}

void NodeFactory::registerNodetype(QString classname, Node::node_t typeInfo,
        NodeInstatiator_t instantiatorFunction)
{
    // Register the ctor for the given classname.
    _registry[classname.toStdString()] = instantiatorFunction;

    // Add it to a list of nodes in the same type, for use
    // by the CueLib widget.
    _registryByType[typeInfo] << classname;
}


// STYLE ? could be: std::shared_ptr<Node> NodeFactory::instatiateNode(QString name)
// Given the name of a node type, finds its ctor in the registry,
// and instantiates an instance of that node.
Node * NodeFactory::instatiateNode(QString classname, QUuid uuid)
{
    Node *instance = nullptr;

    // find name in the registry and call factory method.
    NodeInstatiator_t instancer = _registry[classname.toStdString()];
    if (! instancer)
        return nullptr;
        // ErrorHandling

    instance = instancer();
    // When we're reading in from a file, there will already be a uuid.
    // If we call this with a uuid, then assign it:
    if (! uuid.isNull())
        instance->_uuid = uuid;  // GROSS this keeps uuid from being const.
    instance->_className = classname; // GROSS should get classname automatically in ctor
    _allNodes.append(instance); // XXX Hmmm, this might be a good place to use a weak ptr.

#if 0
    // wrap instance in a shared ptr and return
    // XXX not sure if I'm handling the shared_ptr stuff correctly.
    // also not sure if it's needed.
    if (instance != nullptr)
        return std::shared_ptr<Node>(instance);
    else
        return nullptr;
#else
    return instance;
#endif
}

NodeFactory * NodeFactory::Singleton() {
    // First, the NodeFactory obviously needs to be a singleton, since
    // we don't want/need a bunch of separate factories or registries!
    //
    // We need to implement it this way--rather than just using a single
    // instance of NodeFactory--
    // so that we get the right order of initialization, and the Registrar
    // won't be trying to access an uninitialized factory.
    static NodeFactory factory;
    return &factory;
}

// Return all nodes of a given type.
const QStringList & NodeFactory::getNodesOfType(Node::node_t typeInfo) {
    return _registryByType[typeInfo];
}


// -------------------
// File writing

bool NodeFactory::saveToFile(QString filename)
{
    QFile qfile(filename);
    if (!qfile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open file for saving."); //ErrorHandling
        return false;
    }

    QJsonObject sceneObject;
    // Fill sceneObject with all the scene data.
    writeToJSONObj(sceneObject);

    QJsonDocument jsonDoc(sceneObject);
    qfile.write(jsonDoc.toJson());

    return true;
}

void NodeFactory::writeToJSONObj(QJsonObject &json) const
{

    QJsonArray nodeJsonArray;
    foreach (const Node *node, _allNodes) {
         QJsonObject nodeJson;
         node->writeToJSONObj(nodeJson);
         nodeJsonArray.append(nodeJson);
     }
     json["nodes"] = nodeJsonArray;

     // TODO write:
     //     audioFilename
     //     onset, beat, etc., files
     //     UI settings

}

// -------------------
// File reading

bool NodeFactory::readFromFile(QString filename)
{
    QFile qfile(filename);
    if (!qfile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open saved file.");
        return false;
    }

    QByteArray fileData = qfile.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(fileData));

    readAllNodes(jsonDoc.object());

    return true;
}

void NodeFactory::readAllNodes(const QJsonObject &json)
{
    // TODO something like this:
    //CueSheet.clear();
    // or
    //CueSheet.new();
    // or maybe Nodefactory::{new,clear}....
    // Or maybe this should be a switch in the file open dialog?

    QJsonArray nodesArray = json["nodes"].toArray();
    for (int i = 0; i < nodesArray.size(); ++i) {
        QJsonObject nodeObject = nodesArray[i].toObject();
        readNodeFromJSONObj(nodeObject);
    }

    // Make connections
    foreach (ParamBase* param, _connectionsToMake.keys()) {
        QUuid serverUuid =  _connectionsToMake[param];
        ParamBase* server = _registryUUIDtoParam[serverUuid];
        param->connectTo(server);
    }
}

void NodeFactory::readNodeFromJSONObj(const QJsonObject &json)
{
    QString classname   = json["classname"].toString();
    QUuid uuid          = QUuid(json["uuid"].toString());

    Node * newnode      = Singleton()->instatiateNode(classname, uuid);
    newnode->readFromJSONObj(json);

    QJsonArray paramsArray = json["paramList"].toArray();
    for (int i=0; i<paramsArray.size(); ++i) {
        QJsonObject paramJsonObject = paramsArray[i].toObject();
        // Params don't need to be instantiated, since they are not dynamic;
        // any given node will automatically instantiate all its params.
        // But their values, UUIDs, and connections must be read and assigned appropriately.
        QString paramname = paramJsonObject["name"].toString();

        // Find a pointer to the named param in this node:
        ParamBase *param = newnode->getParamByName(paramname);

        // Read values, which is specialized by type.
        param->readFromJSONObj(paramJsonObject);

        // Store UUID in registry
        _registryUUIDtoParam[param->getUuid()] = param;

        // If param has a connection, deal with that:
        if (paramJsonObject.contains("connectedTo")) {
            QUuid connectedUUID = QUuid(paramJsonObject["connectedTo"].toString());
            // Since connections will be made to parameters that may not have been
            // instantiated yet, we need to save all these connections, then
            // make them after everything has been read and instatiated.
            _connectionsToMake[param] = connectedUUID;
        }
    }
}
