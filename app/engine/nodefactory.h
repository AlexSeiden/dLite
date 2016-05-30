#ifndef NODEFACTORY_H
#define NODEFACTORY_H
#include "nodes/Node.h"

// ------------------------------------------------------------------------------
// NodeFactory
// Singleton class to list & instantiate all available nodes.
class NodeFactory
{

public:
    NodeFactory();
    ~NodeFactory();

    typedef  std::function<Node*(void)> NodeInstatiator_t;

    Node*           instatiateNode(QString classname, QUuid uuid=0);
    void            registerNodetype(QString classname,
                                     Node::node_t typeInfo,
                                     NodeInstatiator_t instantiatorFunction);
    const QStringList& getNodesOfType(Node::node_t typeInfo);
    QList<Node*>    allNodes() const {return Node::allNodes();}
    void            duplicateNodes(QList<Node *> *dupeThese, QRectF bbox = QRectF());

    static NodeFactory* Singleton();

    // Serialization
    bool    saveToFile(QString filename);
    bool    readFromFile(QString filename, bool ignoreCuesheets=false);
    Node*   readNodeFromJSONObj(const QJsonObject &json);
    void    readCuesheets(const QJsonObject &json, bool ignoreCuesheets=false);
    void    readAllNodes(const QJsonObject &json);
    void    writeNodesToJSONObj(QJsonObject &json, QList<Node *> nodes) const;
    void    writeCuesheetsToJSONObj(QJsonObject &json) const;

private:
    // Registry
    std::map<std::string, NodeInstatiator_t>    m_registry;
    QMap<Node::node_t, QStringList>             m_registryByType;

    // Used when reading from a file
    QMap<QUuid, ParamBase*>                     m_uuidToParam;
    QMap<ParamBase *, QUuid>                    m_connectionsToMake;
    QMap<QUuid, QUuid>                          m_groupsToMake;

    bool            m_dirty;
};


// ------------------------------------------------------------------------------
// Registrar
// Called to add each kind of node to the registry.

template<class T>
class Registrar {
public:
    Registrar(QString className, Node::node_t nodeType) {
        // register the ctor for a Node subclass, so that
        // the factory can instatiate them by name.
        NodeFactory::Singleton()->registerNodetype(className, nodeType,
                                      [](void) -> Node * { return new T();});
    }
};

#endif // NODEFACTORY_H
