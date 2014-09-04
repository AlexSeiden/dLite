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

class ParamBase
{
public:
    ParamBase() : _isOutput(false), _isConnectable(true) {}
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

    // Would it be useful to maintain a linked list of connections here?
    // Both for inputs and outputs?  Would that break modularity?
    //QList<ParamBase *>  getConnections();

private:
    QString _name;
    bool _isOutput;
    bool _isConnectable;

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
    Param(PT value=PT()) : _value(value), _provider(nullptr) {}
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
        // Assumes this implements operator() to create closure/functor.
        return [this](PT &val) {(*this)(val);};
    }

    // ??? Should this automatically disable a _provider functor, if there is one?
    // Or should setValue be disabled when there is a provider?
    void setValue(const PT &value) {_value = value;}

private:
    PT _value;
    std::function<void(PT &value)> _provider;
};


// For convinience & speed when doing RTTI
extern const std::type_info & paramTypeFloat;
extern const std::type_info & paramTypeInt;
extern const std::type_info & paramTypeLcolor;
#endif // PARAM_H
