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
#include <QDebug>
#include <QRectF>


// ------------------------------------------------------------------------------
// Node
// Abstract base class for all Nodes

class NodeFactory;
class NodeItem;

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

    bool                isActive() const;
    void                setActive(bool active);

    virtual node_t      getType();
    QList<ParamBase *>  getParams()         {return _paramList;}
    virtual QString     getClass() const    {return _className;}

    NodeItem*           getNodeItem() const         {return _nodeItem;}
    void                setNodeItem(NodeItem* ni)   {_nodeItem= ni;}

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    virtual void operator() (void) = 0;

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
    static bool nameIsUnique(QString name);
    static QString uniqueName(QString name);

    // Called by editor widgets when a parameter has been changed.
    // Most nodes don't need this--only onces like RandomInt where
    // setup has to be re-run when parameters change.
    virtual void        paramHasBeenEdited() {}

    // Only needed on Nodes that have special editor widgets
    // (such as the SubrangeNode) where selection matters to other
    // widgets in editors.  (GROSS--a view thing in the model?)
    virtual void        beenSelected();
    virtual void        beenDeselected();

    // Only needed on Nodes that use files stored with the song
    // (such as the beat nodes).
    virtual void        loadFile();
    virtual void        loadFile(QString filename);

    // Boilerplate call in every operator(); checks to make sure
    // that the operator hasn't been run already for a given frame.
    // This would not only be wasteful, but may well be wrong.
    virtual bool        evaluatedThisFrame();

    // Serialization
    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;
    static QList<Node*>    allNodes() {return _allNodes;}

protected:
    ParamBase *         getParamByName(QString paramname);

    void                evalAllInputs();

    void                cloneHelper(Node &lhs);

    QString               _name;
    QList<ParamBase *>    _paramList;
    QMap<QString, ParamBase*> _paramDict;
    node_t                _type;
    int                   _frameLastEvaluated;
    /*const */QUuid       _uuid;        // Could be const except for need to assign in NodeFactory::instantiateNode when reading from file.
    QString               _className;   // Assigned by NodeFactory
    NodeItem*             _nodeItem;


    static QList<Node *>  _allNodes;

    // New style params
    template <typename T>
    Param<T> *addParam(QString name, const T& defValue=T(0), bool output=false, bool connectable=true)
    {
        Param<T>* param = new Param<T>;
        param->setName(name);
        param->setOutput(output);
        param->setConnectable(connectable);
        param->setValue(defValue);

        param->setParentNode(this);
        _paramList << param;
        if (_paramDict.contains(name)) {
            //ErrorHandling
            qDebug() << Q_FUNC_INFO << QString("adding parameter \"%1\" to node \"%2\" more than once").arg(name, _name);
        }
        _paramDict[name] = param;

        return(param);
    }

    template <typename T>
    void getValue(QString paramName, T& value) const
    {
        ParamBase *pb = _paramDict[paramName];
        Q_ASSERT_X(pb, "No param named ", paramName.toStdString().c_str());
        Param<T>*param = dynamic_cast<Param<T>*>(pb);
        Q_ASSERT(param);
        param->getValue(value);
    }

    template <typename T>
    void setValue(QString paramName, const T value)
    {
        ParamBase *pb = _paramDict[paramName];
        Q_ASSERT_X(pb, "No param named ", paramName.toStdString().c_str());
        Param<T>*param = dynamic_cast<Param<T>*>(pb);
        Q_ASSERT(param);
        param->setValue(value);
    }

    // This is so instantiateNode can set classname:
    friend class NodeFactory;
};

#endif // NODE_H
