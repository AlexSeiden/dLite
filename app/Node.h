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
    enum node_t {CUE, FLOAT, INT, COLOR, BEAT, POSITION, REGION, UNDEFINED};
    // The node types defined:
    //  CUE     --  Called directly by the dance floor; can set values on the floor.
    //  FLOAT   --  A floating point numeric value.
    //  INT     --  An integer numeric value.
    //  COLOR   --  A color
    //  BEAT    --  A boolean "trigger" value, that goes momentarily "true" when a
    //              condition is met.  e.g., every beat or measure
    //  POSITION--  May be obsolete? An x,y postion...could be int or float...
    //              Provides an x & y output that need to be independently connected.
    //  REGION  --  A selected region of cells.
    //  UNDEFINED -- Shouldn't happen; only a temporary designation when the abstract
    //               Node class has initialized, but the concrete class has not.

    Node();
    virtual ~Node();

    virtual Node*       clone() = 0;

    const QString &     getName() const {return _name;}
    void                setName(const QString name) {_name = name;}

    bool                isActive() const {return _active._value;}
    void                setActive(bool active) {_active._value = active;}

    virtual node_t      getType();
    QList<ParamBase *>  getParams()        {return _paramList;}
    virtual QString     getClass() const   {return _className;}

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    virtual void operator() (void) = 0;

//
#if 0
    // LATER:   Suggested by Myers item 35
    void operator() (void) {
        if (evaluatedThisFrame())
            return;
        evalAllInputs();

        doEvalOperation();

        // Boilerplate end of operator:
        _output._qvOutput.setValue(_output._value);
    }
private:
    virtual void doEvalOperation() = 0;
#endif

    static int nodeCount() {return _allNodes.size();}

    // Called by editor widgets when a parameter has been changed.
    // Most nodes don't need this--only onces like RandomInt where
    // setup has to be re-run when parameters change.
    virtual void        paramHasBeenEdited() {}

    // Only needed on Nodes that have special editor widgets
    // (such as the SubrangeNode) where selection matters to other
    // widgets in editors.  (GROSS--a view thing in the model?)
    virtual void        beenSelected();
    virtual void        beenDeselected();

    // Boilerplate call in every operator(); checks to make sure
    // that the operator hasn't been run already for a given frame.
    // This would not only be wasteful, but may well be wrong.
    virtual bool        evaluatedThisFrame();

    // Serialization
    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;
    static QList<Node*>    allNodes() {return _allNodes;}

protected:
    // Boilerplate call in every ctor; sets a pointer from
    // each param back to the its parent node.
    void                setParamParent();
    ParamBase *         getParamByName(QString paramname);

    void                evalAllInputs();

    void                cloneHelper(Node &lhs);

    QString               _name;
    Param<bool>           _active;
    QList<ParamBase *>    _paramList;
    QMap<QString, ParamBase*> _paramDict;
    node_t                _type;
    int                   _frameLastEvaluated;
    /*const */QUuid       _uuid;        // Could be const except for need to assign in NodeFactory::instantiateNode when reading from file.
    QString               _className;   // Assigned by NodeFactory


    static QList<Node *>  _allNodes;

    // New style params
    template <typename T>
    void addParam(QString name, const T& defValue=T(0), bool output=false, bool connectable=true)
    {
        Param<T>* param = new Param<T>;
        param->setName(name);
        param->setOutput(output);
        param->setConnectable(connectable);
        param->setValue(defValue);

        param->setParentNode(this);
        _paramList << param;
        _paramDict[name] = param;
    }

    template <typename T>
    void getValue(QString paramName, T& value)
    {
        Param<T>*param = dynamic_cast<Param<T>*>(_paramDict[paramName]);
        Q_ASSERT(param);
        param->getValue(value);
    }

    template <typename T>
    void setValue(QString paramName, T& value)
    {
        Param<T>*param = dynamic_cast<Param<T>*>(_paramDict[paramName]);
        Q_ASSERT(param);
        param->setValue(value);
    }

    // This is so instantiateNode can set classname:
    //friend Node* NodeFactory::instatiateNode(QString classname);
    friend class NodeFactory;
};

//
// ------------------------------------------------------------------------------
// NodeFactory
// Singleton class to list & instantiate all available nodes.
class NodeFactory
{
public:
    NodeFactory();

    typedef  std::function<Node*(void)> NodeInstatiator_t;

    Node*           instatiateNode(QString classname, QUuid uuid=0);
    void            registerNodetype(QString classname, Node::node_t typeInfo, NodeInstatiator_t instantiatorFunction);
    const QStringList& getNodesOfType(Node::node_t typeInfo);
    QList<Node*>    allNodes() const {return Node::allNodes();}
    void            duplicateNodes(QList<Node*> dupeThese);

    static NodeFactory *Singleton();

    // Serialization
    bool    saveToFile(QString filename);
    bool    readFromFile(QString filename);
    Node*   readNodeFromJSONObj(const QJsonObject &json);
    void    readAllNodes(const QJsonObject &json);
    void    writeToJSONObj(QJsonObject &json) const;

private:
    // Registry
    std::map<std::string, NodeInstatiator_t>    _registry;
    QMap<Node::node_t, QStringList>             _registryByType;
    QMap<QUuid, ParamBase*>                     _registryUUIDtoParam;
    QMap<ParamBase *, QUuid>                    _connectionsToMake;

    bool            _dirty;     // TODO set this, and maybe move it to static node
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
