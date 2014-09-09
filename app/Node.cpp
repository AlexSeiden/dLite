#include "Node.h"
#include "utils.h"

// ------------------------------------------------------------------------------
//  Node
//  base class

int Node::_nodeCount = 0;

Node::Node() :
    _name(QString()),
    _active(true),
    _type(UNDEFINED)
{
    _nodeCount++;
}

Node::node_t Node::getType()
{
    return _type;
}

Node::~Node() {}

void Node::beenSelected() {}

// XXX gross.  at least, should be a better way to init this.
// should be a better way to find out without having to maintain this.
// kinda breaks encapsulation to have it in the first place.
void Node::setParamParent()
{
    foreach (ParamBase *p, _paramList) {
        p->_parentNode = this;
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
// ------------------------------------------------------------------------------
//  Node Factory

NodeFactory::NodeFactory() {}

void NodeFactory::registerNodetype(QString name, Node::node_t typeInfo,
        NodeInstatiator_t instantiatorFunction)
{
    // register the class factory function
    _registry[name.toStdString()] = instantiatorFunction;
    _registryByType[typeInfo] << name;

}

#if 0
std::shared_ptr<Node> NodeFactory::instatiateNode(QString name)
#else
Node * NodeFactory::instatiateNode(QString name)
#endif
{
    Node *instance = nullptr;

    // find name in the registry and call factory method.
    NodeInstatiator_t instancer = _registry[name.toStdString()];
    if (! instancer)
        return nullptr;
        // TODO error

    instance = instancer();
    _allNodes.append(instance); // XXX Hmmm, this might be a good place to use a weak ptr.

#if 0
    // wrap instance in a shared ptr and return
    // XXX not sure if I'm handling the shared_ptr stuff correctly
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

