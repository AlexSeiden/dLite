#ifndef PARAM_H
#define PARAM_H

#include <QList>
#include <QString>
#include <functional>
#include <typeinfo>
#include "lightcolor.h"

// Classes that encapsulate individual parameters on Nodes & Cues.

// ParamBase
//      An abstract base class.

class Node;

class ParamBase
{
public:
    ParamBase() : _isOutput(false), _isConnectable(true) , _type(typeid(this)) {}
    virtual ~ParamBase() {}

//    virtual void setProvider(std::function<void(void *)> provider) = 0;
//    virtual std::function<void(void *)> getProvider() = 0;

    QString getName()               {return _name;}
    void    setName(QString name)   {_name = name;}
    bool    isOutput()              {return _isOutput;}
    bool    isConnectable()         {return _isConnectable;}
    void    setOutput(bool status)             {_isOutput = status;}
    void    setConnectable(bool status)        {_isConnectable = status;}

    bool    isConnectableTo(ParamBase *otherParam);
    void    connectParams(ParamBase *server, ParamBase *client);
    void    connectTo(ParamBase *server);

    virtual const std::type_info & getType() {return _type;}

    // Would it be useful to maintain a linked list of connections here?
    // Both for inputs and outputs?  Would that break modularity?
    //QList<ParamBase *>  getConnections();

    Node *getParent() {return _parentNode;}

    // Valid only for input nodes
    Node *getServer() {return _connectedNode;}

    // Valid only for output nodes
    Node *getClients() {return _connectedNode;}

    Node *_connectedNode;
    Node *_parentNode;
protected:
    QString _name;
    bool _isOutput;
    bool _isConnectable;
    const std::type_info & _type;
};


// Templated Param class
//      Instatiated with the following types:
//          float
//          int
//          Lightcolor
//          bool (for triggers)
//          positions
//          regions

template <class PT>
class Param : public ParamBase
{
public:
    Param(PT value=PT()) :
        _value(value),
        _provider(nullptr) {}


    virtual ~Param() {}

    virtual void operator() (PT &value) {value = _value;}

    virtual void getValue(PT &value)  {
        // update current member value, if provider is connected
        if (_provider)
            _provider(_value);

        // All values are returned by reference, even for fundamental types like ints and
        // floats, for generality with all types, such as Lightcolor & regions.
        value = _value;
    }

    // This applies to input parameters only: it sets the provider "function"
    // that is excuted to compute the value of this parameter.
    void setProvider(std::function<void(PT&)> provider) {_provider = provider;}

    // This applies to output parameters only:  it returns a provider "function"
    // that can be executed by a client to compute the value the client uses.
    std::function<void(PT&)> getProvider() {
        // Assumes that the node implements operator() to create closure/functor.
        return [this](PT &val) {(*this->_parentNode)(val);};
//        return [this](PT &val) {(*this)(val);};
    }

    // ??? Should this automatically disable a _provider functor, if there is one?
    // Or should setValue be disabled when there is a provider?
    void setValue(const PT &value) {_value = value;}

    virtual const std::type_info & getType() {return typeid(this);}

private:
    PT _value;
    std::function<void(PT &value)> _provider;
};


// For convinience & speed when doing RTTI
extern const std::type_info & paramTypeFloat;
extern const std::type_info & paramTypeInt;
extern const std::type_info & paramTypeLcolor;
#endif // PARAM_H
