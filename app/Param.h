#ifndef PARAM_H
#define PARAM_H

#include <QList>
#include <QString>
#include <QVariant>
#include <QUuid>
#include <functional>
#include <typeinfo>
#include "lightcolor.h"

// Classes that encapsulate individual parameters on Nodes & Cues.

// ParamBase
//      An abstract base class.

class Node;
class QJsonObject;

class ParamBase
{
public:
    ParamBase() :
        _provider(nullptr),
        _connectedParam(nullptr),
        _isOutput(false),
        _isConnectable(true),
        _type(typeid(this)),
        _uuid(QUuid::createUuid())  { }
    virtual ~ParamBase() { }

    QString     getName() const                {return _name;}
    void        setName(QString name)          {_name = name;}
    bool        isOutput() const               {return _isOutput;}
    bool        isConnectable() const          {return _isConnectable;}
    void        setOutput(bool status)         {_isOutput = status;}
    void        setConnectable(bool status)    {_isConnectable = status;}
    ParamBase*  connectedParam()               {return _connectedParam;}
    QUuid&      getUuid()                      {return _uuid;}

    bool    isConnectableTo(ParamBase *otherParam);
    void    connectTo(ParamBase *server);
    void    disconnect() {_connectedParam = nullptr; _provider = nullptr;}

    virtual void eval() {
        if (_provider)
            _provider();
        // GROSS.  it evaluates it, but now it's stuck
        // in the providers "output" variable.
    }

    virtual const std::type_info & getType() {return _type;}

    std::function<void()> getProvider();

    // This applies to input parameters only: it sets the provider "function"
    // that is excuted to compute the value of this parameter.
    void setProvider(std::function<void()> provider) {_provider = provider;}

    // Returns a pointer of the Node that this is a parameter for.
    Node *  getParent() {return _parentNode;}
    void    setParent(Node *parent) {_parentNode = parent;}

    // Valid only for input nodes
    Node *getServer() {
        if (_connectedParam)
            return _connectedParam->getParent();
        return nullptr;
    }

    // Serialization
    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

    // Would it be useful to maintain a linked list of connections here?
    // Both for inputs and outputs?  Would that break modularity?
    //QList<ParamBase *>  getConnections();
    // Would be valid only for output nodes
    //Node *getClients();

    // Any OUTPUT params need to stick their value in here.
    // Kinda GROSS but easiest way to (AFAIK) to pass arbitray datatypes.
    QVariant    _qvOutput;

protected:
    std::function<void()>   _provider;
    ParamBase *             _connectedParam;
    Node *                  _parentNode;      // The node that this param is a part of.

    QString                 _name;
    bool                    _isOutput;
    bool                    _isConnectable;
    const std::type_info &  _type;
    /*const*/ QUuid             _uuid; // Can't be const bc of assignment during file read

    friend class NodeFactory;
};


// ------------------------------------------------------------------------------
// Templated Param class
//      Instatiated with the following types:
//          float
//          int
//          Lightcolor
//          bool (for triggers)
//          positions
//          regions

template <class PARAMT>
class Param : public ParamBase
{
public:
    Param(PARAMT value=PARAMT()) :
        _value(value)
        {}

    virtual ~Param() {}

    virtual void getValue(PARAMT &value)  {
        // All values are returned by reference, even for fundamental types like ints and
        // floats, for generality with all types, such as Lightcolor & regions.
        value = _value;
    }

    virtual void eval() {
        if (_provider) {
            _provider();
            _value = _connectedParam->_qvOutput.value<PARAMT>();
        }
    }

    void setValue(const PARAMT &value) {_value = value;}

    // ??? do we need this?  Would it be better just to assign a _type field?
    virtual const std::type_info & getType() {return typeid(this);}

    PARAMT _value;

    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;
};

// For convinience & speed when doing RTTI
extern const std::type_info & paramTypeFloat;
extern const std::type_info & paramTypeInt;
extern const std::type_info & paramTypeLightcolor;
extern const std::type_info & paramTypeBool;
#endif // PARAM_H
