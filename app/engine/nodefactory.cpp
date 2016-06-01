// Copyright (C) 2014-2016 Alex Seiden 
//
#include "engine/nodefactory.h"
#include "views/graph_widget.h"
#include "views/cuesheet_scene.h"
#include "engine/dispatch.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

// ------------------------------------------------------------------------------
//  NodeFactory

NodeFactory::NodeFactory()  {}
NodeFactory::~NodeFactory() {}

void NodeFactory::registerNodetype(
        QString classname,
        Node::node_t typeInfo,
        NodeInstatiator_t instantiatorFunction)
{
    // Register the ctor for the given classname.
    m_registry[classname.toStdString()] = instantiatorFunction;

    // Add it to a list of nodes in the same type, for use
    // by the CueLib widget.
    m_registryByType[typeInfo] << classname;
}


// Given the name of a node type, finds its ctor in the registry,
// and instantiates an instance of that node.
Node * NodeFactory::instatiateNode(QString classname, QUuid uuid)
{
    Node *instance = nullptr;

    // find name in the registry and call factory method.
    NodeInstatiator_t instancer = m_registry[classname.toStdString()];
    if (! instancer)
        return nullptr;

    instance = instancer();
    // When instatiating from the CueLibrary, the uuid will be null,
    // which means a new one should be created.
    // When we're reading in from a file, there will already be a uuid
    // which connections may refer to, and we need to assign it here.
    if (! uuid.isNull())
        instance->m_uuid = uuid;
    instance->m_className = classname;

    return instance;
}

NodeFactory * NodeFactory::Singleton() {
    // The NodeFactory obviously needs to be a singleton, since
    // we don't want/need a bunch of separate factories or registries!
    //
    // We need to implement it this way--rather than just using a single
    // instance of NodeFactory--so that we get the right order of initialization,
    // and the Registrar won't be trying to access an uninitialized factory.
    static NodeFactory factory;
    return &factory;
}

// Return all nodes of a given type.
const QStringList & NodeFactory::getNodesOfType(Node::node_t typeInfo) {
    return m_registryByType[typeInfo];
}

void NodeFactory::duplicateNodes(QList<Node*>* dupeThese, QRectF bbox)
{
    // Duplicate each node
    QMap<Node*, Node*> nodeMapping;
    foreach (Node* orignode, *dupeThese) {
        Node* newnode = orignode->clone();
        Dispatch::Singleton()->getGraphWidget()->addNode(newnode);
        nodeMapping[orignode] = newnode;
    }

    // Connections between the nodes being duplicated need to point to the
    // duplicates, and not the original ones. Connections outside the subnet
    // being duplicated are unchanged.
    QList<ParamBase*> haveInternalConnections;
    QList<ParamBase*> haveExternalConnections;
    foreach (Node* newNode, nodeMapping.values()) {
        foreach (ParamBase* newParam, newNode->getParams()) {
            Node* origServer = newParam->getServer();
            if (! origServer) continue;

            if (dupeThese->contains(origServer)) {
                haveInternalConnections << newParam;
                // Rewire a connection within the network so that it stays within the network
                Node* newServer = nodeMapping[origServer];
                Q_ASSERT(newServer);
                ParamBase* oldConnection = newParam->connectedParam();
                QString name = oldConnection->getName();
                ParamBase* newConnection = newServer->getParamByName(name);
                Q_ASSERT(newConnection);
                newParam->connectTo(newConnection);
            }
            else {
                // Connection has already been made
                haveExternalConnections << newParam;
            }
        }
    }

    // Add external connection items to graph
    // (Purely a display operation)
    foreach (ParamBase* param, haveExternalConnections) {
        Dispatch::Singleton()->getGraphWidget()->addConnection(param->connectedParam(), param);
    }

    foreach (ParamBase* param, haveInternalConnections) {
        Dispatch::Singleton()->getGraphWidget()->addConnection(param->connectedParam(), param);
    }

#if 0
    // TODO Relative positioning.
    // First, pick a position for the new nodes
    //to the right of the bbox of the originals:

    if (bbox.isNull()) return;

    QPointF origOrigin = bbox.topLeft();
    QPointF newOrigin = bbox.topRight() + QPointF(20, 20);
    foreach (Node* origNode, *dupeThese) {
        Node* newNode = nodeMapping[origNode];
        QPointF deltaOrig = origNode->Pos() - origOrigin;  // XXX Need orignodeItem->pos()
        QPointF newPos = newOrigin + deltaOrig;
        newNode->setPos(newPos);        // XXX need newNodeItem->setPos
    }
#endif

    Dispatch::Singleton()->getGraphWidget()->selectNodes(nodeMapping.values());
}

// -------------------
// File writing

// Saves all nodes to JSON file
bool NodeFactory::saveToFile(QString filename)
{
    QFile qfile(filename);
    if (!qfile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open file for saving."); //ErrorHandling
        return false;
    }

    QJsonObject sceneObject;
    // Fill sceneObject with all the scene data.
    writeCuesheetsToJSONObj(sceneObject);

    QJsonDocument jsonDoc(sceneObject);
    qfile.write(jsonDoc.toJson());
    m_dirty = false;

    return true;
}

void NodeFactory::writeNodesToJSONObj(QJsonObject &json, QList<Node*>nodes) const
{
    QJsonArray nodeJsonArray;
    foreach (const Node *node, nodes) {
         QJsonObject nodeJson;
         node->writeToJSONObj(nodeJson);
         nodeJsonArray.append(nodeJson);
    }
    json["nodes"] = nodeJsonArray;
}

void NodeFactory::writeCuesheetsToJSONObj(QJsonObject &json) const
{
    QJsonArray cuesheetJsonArray;
    foreach (const CuesheetScene *cuesheet,
             Dispatch::Singleton()->getGraphWidget()->getCuesheets()) {
         QJsonObject cuesheetJson;
         cuesheetJson["name"] = cuesheet->getName();
         writeNodesToJSONObj(cuesheetJson, cuesheet->getNodes());
         cuesheetJsonArray.append(cuesheetJson);
    }
    json["cuesheets"] = cuesheetJsonArray;
}

// -------------------
// File reading

bool NodeFactory::readFromFile(QString filename, bool ignoreCuesheets)
{
    QFile qfile(filename);
    if (!qfile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open saved file.");
        return false;
    }

    QByteArray fileData = qfile.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(fileData));

    readCuesheets(jsonDoc.object(), ignoreCuesheets);

    return true;
}

void NodeFactory::readCuesheets(const QJsonObject &json, bool ignoreCuesheets)
{
    QJsonObject::ConstIterator cuesheetIter = json.constFind("cuesheets");
    // If there are no cuesheets in the file, just read the nodes:
    if (cuesheetIter == json.constEnd()) {
        readAllNodes(json);
        return;
    }

    QJsonArray cuesheetsArray = (*cuesheetIter).toArray();
    for (int i = 0; i < cuesheetsArray.size(); ++i) {
        QJsonObject cuesheetObject = cuesheetsArray[i].toObject();
        if (! ignoreCuesheets) {
            QString name = cuesheetObject["name"].toString();
            Dispatch::Singleton()->getGraphWidget()->newCuesheet(name);
        }
        readAllNodes(cuesheetObject);
    }
    Dispatch::Singleton()->getGraphWidget()->deleteEmptyFirstCuesheet();

}

void NodeFactory::readAllNodes(const QJsonObject &json)
{
    m_connectionsToMake.clear();
    m_groupsToMake.clear();
    m_uuidToParam.clear();

    QJsonArray nodesArray = json["nodes"].toArray();
    // Instatiate a node for every node in the json file.
    // Keep track of any connections that will need to be made in
    // the _connectionsToMake[] map.
    for (int i = 0; i < nodesArray.size(); ++i) {
        // Parse and instatiate each node:
        QJsonObject nodeObject = nodesArray[i].toObject();
        Node* newnode = readNodeFromJSONObj(nodeObject);

        // Add to the graph, and restore graph position:
        Dispatch::Singleton()->getGraphWidget()->addNode(newnode, &nodeObject);
    }

    // Make connections between nodes:
    foreach (ParamBase* param, m_connectionsToMake.keys()) {
        QUuid serverUuid =  m_connectionsToMake[param];
        ParamBase* server = m_uuidToParam[serverUuid];
        param->connectTo(server);
        Dispatch::Singleton()->getGraphWidget()->addConnection(server, param);
    }
}

Node* NodeFactory::readNodeFromJSONObj(const QJsonObject &json)
{
    QString classname   = json["classname"].toString();
    QUuid uuid          = QUuid(json["uuid"].toString());

    Node* newnode      = Singleton()->instatiateNode(classname, uuid);
    newnode->readFromJSONObj(json);

    // Read all parameters
    QJsonArray paramsArray = json["paramList"].toArray();
    for (int i=0; i<paramsArray.size(); ++i) {
        QJsonObject paramJsonObject = paramsArray[i].toObject();
        QString paramname = paramJsonObject["name"].toString();

        // Find a pointer to the named param in this node:
        ParamBase *param = newnode->getParamByName(paramname);

        if (! param) {
            qDebug() << Q_FUNC_INFO << QString("Node \"%1\" has no parameter \"%2\"").arg(classname, paramname);
            continue;
        }

        // Read values, which is specialized by type.
        param->readFromJSONObj(paramJsonObject);

        // Store UUID in registry, so later when we're hooking things up,
        // we can connect a reference to a UUid to the parameter that's been
        // instatiated for it.
        m_uuidToParam[param->getUuid()] = param;

        // If param has a connection, deal with that:
        if (paramJsonObject.contains("connectedTo")) {
            QUuid connectedUUID = QUuid(paramJsonObject["connectedTo"].toString());
            // Since connections will be made to parameters that may not have been
            // instantiated yet, we need to save all these connections, then
            // make them after everything has been read and instatiated.
            m_connectionsToMake[param] = connectedUUID;
        }
    }

    return newnode;
}
