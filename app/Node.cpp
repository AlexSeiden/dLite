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

QList<Node*> Node::_allNodes;

Node::Node() :
    _name(QString()),
    _active(true),
    _type(UNDEFINED),
    _uuid(QUuid::createUuid())
{
    _allNodes.append(this);
}

Node::~Node() {
    _allNodes.removeAll(this);
}

// The "beenSelected" methods are virtual so that nodes like Sublevels
// & Paths can communicate with special editors.
void Node::beenSelected() {}
void Node::beenDeselected() {}

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

// evalAllInputs()
//      Evaluates the providers connected to any input parameters.
void Node::evalAllInputs()
{
    foreach (ParamBase *p, getParams()) {
        if (! p->isOutput()) {
            p->eval();
        }
    }
}

// evaluatedThisFrame()
//      Predicate that returns true if the node has already been evaluated
//      on this frame, and therefore shouldn't be evaluted again.
bool Node::evaluatedThisFrame()
{
    int frame =  Cupid::getCurrentFrame();
    if (frame == _frameLastEvaluated)
        return true;

    _frameLastEvaluated = frame;
    return false;
}

// Serialization
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
//      Utility function that's used when reading a file from disk.
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
//  NodeFactory

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
    // When instatiating from the CueLibrary, the uuid will be null,
    // which means a new one should be created.
    // When we're reading in from a file, there will already be a uuid
    // which connections may refer to, and we need to assign it here.
    if (! uuid.isNull())
        instance->_uuid = uuid;  // GROSS this keeps uuid from being const.
    instance->_className = classname; // GROSS should get classname automatically in ctor

    // Add to list of all nodes instantiated so far.
    // XXX Hmmm, this might be a good place to use a weak ptr.
    // Alternatively, would it be better to maintain this list as a static
    // member of the Node class, and have the ctors and dtors deal with this?
    // I think yes.
//    _allNodes.append(instance);

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
    _dirty = false;

    return true;
}

void NodeFactory::writeToJSONObj(QJsonObject &json) const
{
    QJsonArray nodeJsonArray;
    foreach (const Node *node, allNodes()) {
         QJsonObject nodeJson;
         node->writeToJSONObj(nodeJson);
         nodeJsonArray.append(nodeJson);
     }
     json["nodes"] = nodeJsonArray;

     // TODO write:
     //     audioFilename
     //     onset, beat, etc., files
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

// XXX right now, we read all the nodes then instatiate all the
// graph stuff.  but to restore saved ui from the json file,
// it'd be easier to have the graph nodes instatiated as-we-go-along.
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
        Node* newnode = readNodeFromJSONObj(nodeObject);
        Cupid::Singleton()->getGraphWidget()->addNode(newnode, &nodeObject);
    }

    // Make connections
    foreach (ParamBase* param, _connectionsToMake.keys()) {
        QUuid serverUuid =  _connectionsToMake[param];
        ParamBase* server = _registryUUIDtoParam[serverUuid];
        param->connectTo(server);
        Cupid::Singleton()->getGraphWidget()->addConnection(server, param);
    }
}

Node* NodeFactory::readNodeFromJSONObj(const QJsonObject &json)
{
    QString classname   = json["classname"].toString();
    QUuid uuid          = QUuid(json["uuid"].toString());

    Node* newnode      = Singleton()->instatiateNode(classname, uuid);
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

    return newnode;
}
