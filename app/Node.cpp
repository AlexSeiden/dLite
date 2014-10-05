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

QList<Node*> Node::_allNodes;

Node::Node() :
    _name(QString()),
    _type(UNDEFINED),
    _frameLastEvaluated(-1),
    _uuid(QUuid::createUuid())
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
    // (this is possibly redundant?  will implicit copy constructor do this?)
    lhs._type       = _type;
    lhs._className  = _className;

    // Things that are modified
    lhs._name     = _name + "COPY";  // XXX better uniquenames
    lhs._frameLastEvaluated = -1;
    lhs._uuid     = QUuid::createUuid();

    // Copy parameter values and connections.
    // If connections are within a group that is being
    // copied together, the caller must handle making the appropriate
    // reconnections.
    foreach (ParamBase* lhsParam, lhs._paramList) {
        ParamBase* rhsParam = getParamByName(lhsParam->getName());
        lhsParam->copyValue(rhsParam);
    }

}

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
        p->setParentNode(this);
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

// ---------------------------
// Serialization
void Node::readFromJSONObj(const QJsonObject &json)
{
    setName(json["name"].toString());
    setActive(json["active"].toBool());
}

void Node::writeToJSONObj(QJsonObject &json) const
{
    // ErrorHandling

    json["name"] = getName();
//    json["active"] = isActive();
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
#if 0  // TODO change this over
    // Return the parameter called "paramname"
    foreach (ParamBase *p, _paramList) {
        if (paramname == p->getName())
            return p;
    }
    // ErrorHandling
    return nullptr;
#else
    if (_paramDict.contains(paramname))
        return _paramDict[paramname];
    return nullptr;
    // ErrorHandling
#endif
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

    return instance;
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

void NodeFactory::duplicateNodes(QList<Node*> dupeThese)
{

    // Duplicate each node
    QMap<Node*, Node*> nodeMapping;
    foreach (Node* orignode, dupeThese) {
        Node* newnode = orignode->clone();
        Cupid::Singleton()->getGraphWidget()->addNode(newnode);
        nodeMapping[orignode] = newnode;
    }

    // separate all connections into those that are external
    // (and need to persist) and those that are internal with the network
    // being copied (and should be duplicated)
    QList<ParamBase*> haveInternalConnections;
    QList<ParamBase*> haveExternalConnections;
    foreach (Node* node, nodeMapping.values()) {
        foreach (ParamBase* param, node->getParams()) {
            Node* server = param->getServer();
            if (! server) continue;

            if (dupeThese.contains(server)) {
                haveInternalConnections << param;
                // Rewire a connection within the network so that it stays within the network
                Node* newServer = nodeMapping[server];
                QString name = param->getName();
                ParamBase* newConnection = newServer->getParamByName(name);
                param->connectTo(newConnection);
            }
            else
                haveExternalConnections << param;
        }
    }

    // Add external connection items to graph
    // (Purely a display operation)
    foreach (ParamBase* param, haveExternalConnections) {
        Cupid::Singleton()->getGraphWidget()->addConnection(param->connectedParam(), param);
    }

    foreach (ParamBase* param, haveInternalConnections) {
        Cupid::Singleton()->getGraphWidget()->addConnection(param->connectedParam(), param);
    }

    // TODO Relative positioning.
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

void NodeFactory::readAllNodes(const QJsonObject &json)
{
    // TODO something like this:
    //CueSheet.clear();
    // or
    //CueSheet.new();
    // or maybe Nodefactory::{new,clear}....
    // Or maybe this should be a switch in the file open dialog?

    QJsonArray nodesArray = json["nodes"].toArray();
    // Instatiate a node for every node in the json file.
    // Keep track of any connections that will need to be made in
    // the _connectionsToMake[] map.
    for (int i = 0; i < nodesArray.size(); ++i) {
        // Parse and instatiate each node:
        QJsonObject nodeObject = nodesArray[i].toObject();
        Node* newnode = readNodeFromJSONObj(nodeObject);

        // Add to the graph, and restore graph position:
        Cupid::Singleton()->getGraphWidget()->addNode(newnode, &nodeObject);
    }

    // Make connections between nodes:
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
        // Params are already instantiated by the node, at least for now.
        // (This will change if we make nodes like Palette allow dynamically adding
        // parameters.)
        // But their values, UUIDs, and connections must be read and assigned appropriately.
        QString paramname = paramJsonObject["name"].toString();

        // Find a pointer to the named param in this node:
        ParamBase *param = newnode->getParamByName(paramname);

        if (! param) {
            // ErrorHandling
            qDebug() << Q_FUNC_INFO << QString("Node \"%1\" has no parameter \"%2\"").arg(classname, paramname);
            continue;
        }

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
