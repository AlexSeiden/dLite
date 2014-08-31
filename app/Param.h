#ifndef PARAM_H
#define PARAM_H

#include <QList>
#include <QString>
#include <functional>
#include <typeinfo>

class ParamBase
{
public:
    ParamBase() : _isOutput(false), _isConnectable(true) {}
    // make this virtual to allow RTTI
    virtual ~ParamBase() {}

    QString getName()               {return _name;}
    void    setName(QString name)   {_name = name;}
    bool    isOutput()              {return _isOutput;}
    bool    isConnectable()         {return _isConnectable;}
    void    setOutput(bool status)             {_isOutput = status;}
    void    setConnectable(bool status)        {_isConnectable = status;}

private:
    QString _name;
    bool _isOutput;
    bool _isConnectable;
};

template <class PT>
class Param : public ParamBase
{
public:
    Param(PT value=PT()) : _value(value), _provider(nullptr) {}
    virtual ~Param() {} // Override to allow RTTI

    void getValue(PT &value)  {
        // update current member value, if provider is connected
        if (_provider)
            _provider(_value);

        // Return by reference, for generality with all types.
        value = _value;
    }

    void setProvider(std::function<void(PT&)> provider) {_provider = provider;}

    // ??? Should this automatically disable a _provider functor, if there is one?
    // or should it be disabled when there is a provider?
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
