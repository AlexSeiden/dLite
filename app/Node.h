#ifndef NODE_H
#define NODE_H
#include "Param.h"
#include <random>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
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

    void                setParamParent();


protected:
    QString               _name;
    bool                  _active;
    QList<ParamBase *>    _paramList;       // Could be static
    node_t                _type;
};


class TriggerEvery : public Node
{

public:
    TriggerEvery() {}

    void setTriggerInterval(int interval) {_interval = interval;}
    void operator() (bool &value);

private:
    void    reset() {_value = false;}

    bool    _value;

    // These are in milliseconds
    // Parameters
    int     _interval;
    //int     _refreshOffset; // TODO

    // Internal timekeepers
    int     _lastRefresh;
    int     _nextRefresh;
};

class ColorRamp : public Node
{
public:
    ColorRamp();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() (Lightcolor &value);

private:
    Lightcolor   _value; // XXX is this now redundant to "output"?

    // Parameters
    Param<Lightcolor> _output;
    Param<Lightcolor> _c0;
    Param<Lightcolor> _c1;
    Param<float> _mix;

private:

};

class RandomFloat : public Node
{
public:
    RandomFloat();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() (float &value);

//private:  // XXX
    float   _value;

    // Parameters
    Param<float> _output;
    Param<float> _min;
    Param<float> _max;
//    TriggerEvery _trigger;

private:
    void  setRandomEngine();

    // Random number generator
    std::mt19937 *_randGenerator;
    std::uniform_real_distribution<float> *_distribution;
};

class RandomInt : public Node
{
public:
    RandomInt();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() (int &value);

//private:  // XXX
    int   _value;

    // Parameters
    Param<int> _output;
    Param<int> _min;
    Param<int> _max;
//    TriggerEvery _trigger;

private:
    void  setRandomEngine();

    // Random number generator
    std::mt19937 *_randGenerator;
    std::uniform_int_distribution<int> *_distribution;
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
