#ifndef NODE_H
#define NODE_H
#include "Param.h"
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <functional>


// Abstract base class for all Nodes:
// A Node is an object that generates parameters for Cues.
class Node
{
public:
    enum node_t {CUE, FLOAT, INT, COLOR, BEAT, POSITION, UNDEFINED};

    Node();
    virtual ~Node();

    const QString &     getName() const {return _name;}
    void                setName(const QString name) {_name = name;}

    bool                isActive() {return _active;}
    void                setActive(bool active) {_active = active;}

    virtual node_t      getType();
    QList<ParamBase *>  getParams()        {return _paramList;}

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    virtual void operator() (void) = 0;

    // Called by editor widgets when a parameter has been changed.
    // Most nodes don't need this--only onces like RandomInt where
    // setup has to be re-run when parameters change.
    virtual void paramHasBeenEdited() {}

    // Only needed on Nodes that have special editor widgets
    // (such as the SubrangeNode) where selection matters to other
    // widgets in editors.  (GROSS--a view thing in the model?)
    virtual void        beenSelected();

    // Boilerplate call in every operator(); checks to make sure
    // that the operator hasn't been run already for a given frame.
    // This would not only be wasteful, but may well be wrong.
    virtual bool        evaluatedThisFrame();

    // Boilerplate call in every ctor; sets a pointer from
    // the parameter back to the parent.
    void                setParamParent();

    static  int         _nodeCount;

protected:
    void                evalAllInputs();

    QString               _name;
    bool                  _active;
    QList<ParamBase *>    _paramList;
    node_t                _type;
    int                   _frameLastEvaluated;
};

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
    Node *instatiateNode(QString name);
#endif

    void registerNodetype(QString name, Node::node_t typeInfo, NodeInstatiator_t instantiatorFunction);

    const QStringList & getNodesOfType(Node::node_t typeInfo);

    static NodeFactory *Singleton();

private:
    // Registry
    std::map<std::string, NodeInstatiator_t>  _registry;
    QMap<Node::node_t, QStringList> _registryByType;

    // Inventory
    QList<Node *>   _allNodes;
};

template<class T>
class Registrar {
public:
    Registrar(QString className, Node::node_t nodeType) {
        // register the class factory function
        NodeFactory::Singleton()->registerNodetype(className, nodeType,
                                      [](void) -> Node * { return new T();});
    }
};

#endif // NODE_H
