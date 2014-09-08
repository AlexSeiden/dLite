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

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    virtual void operator() (void) = 0;

    // Called by editor widgets when a parameter has been changed.
    virtual void paramHasBeenEdited() {}

    const QString &     getName() const {return _name;}
    void                setName(const QString name) {_name = name;}

    bool                isActive() {return _active;}
    void                setActive(bool active) {_active = active;}

    virtual node_t      getType();
    QList<ParamBase *>  getParams()        {return _paramList;}

    void                setParamParent();
    virtual void        beenSelected();

    static  int         _nodeCount;


protected:
    void                evalAllInputs();

    QString               _name;
    bool                  _active;
    QList<ParamBase *>    _paramList;
    node_t                _type;
};

class ColorRamp : public Node
{
public:
    ColorRamp();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() ();

private:
    // Parameters
    Param<Lightcolor> _output;
    Param<Lightcolor> _c0;
    Param<Lightcolor> _c1;
    Param<float> _mix;

private:

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
