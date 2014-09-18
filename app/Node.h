#ifndef NODE_H
#define NODE_H
#include "Param.h"
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QUuid>
#include <functional>
#include <QJsonObject>


// ------------------------------------------------------------------------------
// Node
// Abstract base class for all Nodes

class NodeFactory;

class Node
{
public:
    // These types are purely used for classification for the Cuelibrary.
    // Actual node output connection type is dependent on the parameters
    // marked "output", and can include multiples of these types.
    // Style:  that might be a good enough reason for replacing this enum--
    // as well as the _type field--with lists for the Cuelibrary directly.
    enum node_t {CUE, FLOAT, INT, COLOR, BEAT, POSITION, UNDEFINED};

    Node();
    virtual ~Node();

    const QString &     getName() const {return _name;}
    void                setName(const QString name) {_name = name;}

    bool                isActive() {return _active;}
    void                setActive(bool active) {_active = active;}

    virtual node_t      getType();
    QList<ParamBase *>  getParams()        {return _paramList;}

    virtual QString     getClass()         {return _className;}

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    virtual void operator() (void) = 0;

#if 0
    // LATER
    // Suggested by Myers item 35
    void operator() (void) {
        if (evaluatedThisFrame())
            return;
        evalAllInputs();

        doEvalOperation();

        // Boilerplate end of operator:
        // (this was
        _output._qvOutput.setValue(_output._value);
    }
private:
    virtual void doEvalOperation() = 0;
#endif

    // Called by editor widgets when a parameter has been changed.
    // Most nodes don't need this--only onces like RandomInt where
    // setup has to be re-run when parameters change.
    virtual void        paramHasBeenEdited() {}

    // Only needed on Nodes that have special editor widgets
    // (such as the SubrangeNode) where selection matters to other
    // widgets in editors.  (GROSS--a view thing in the model?)
    virtual void        beenSelected();

    // Boilerplate call in every operator(); checks to make sure
    // that the operator hasn't been run already for a given frame.
    // This would not only be wasteful, but may well be wrong.
    virtual bool        evaluatedThisFrame();

    // Serialization
    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

protected:
    // Boilerplate call in every ctor; sets a pointer from
    // the parameter back to the parent.
    void                setParamParent();
    ParamBase *         getParamByName(QString paramname);


    static  int         _nodeCount; // TODO remove this

    void                evalAllInputs();

    QString               _name;
    bool                  _active;
    QList<ParamBase *>    _paramList;
    node_t                _type;
    int                   _frameLastEvaluated;
    /*const */QUuid       _uuid;        // Could be const except for need to assign in NodeFactory::instantiateNode when reading from file.
    QString               _className;

    // This is so instantiateNode can set classname:
    //friend Node* NodeFactory::instatiateNode(QString classname);
    friend class NodeFactory;
};


// ------------------------------------------------------------------------------
// NodeFactory
// Singleton class to list & instantiate all available nodes.
class NodeFactory
{
public:
    NodeFactory();

    typedef  std::function<Node*(void)> NodeInstatiator_t;

#if 0
    std::shared_ptr<Node> instatiateNode(QString name);
#else
    Node *instatiateNode(QString classname, QUuid uuid=0);
#endif

    void registerNodetype(QString classname, Node::node_t typeInfo, NodeInstatiator_t instantiatorFunction);
    const QStringList & getNodesOfType(Node::node_t typeInfo);
    QList<Node*>        allNodes() {return _allNodes;}

    static NodeFactory *Singleton();

    // Serialization
    bool saveToFile(QString filename);
    bool readFromFile(QString filename);
    void readNodeFromJSONObj(const QJsonObject &json);
    void readAllNodes(const QJsonObject &json);
    void writeToJSONObj(QJsonObject &json) const;

private:


    // Registry
    std::map<std::string, NodeInstatiator_t>    _registry;
    QMap<Node::node_t, QStringList>             _registryByType;
    QMap<QUuid, Node*>                          _registryUUIDtoNodep;
    QMap<QUuid, ParamBase*>                     _registryUUIDtoParam;
    QMap<ParamBase *, QUuid>                    _connectionsToMake;

    // Inventory
    // GROSS Maybe this belongs to dancefloor???
    QList<Node *>   _allNodes;
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

#endif // NODE_H
