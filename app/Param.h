#ifndef PARAM_H
#define PARAM_H

#include <QString>
#include <functional>
#include <typeinfo>

class ParamBase
{
public:
    ParamBase() {}
    // make this virtual to allow RTTI (???)
    virtual ~ParamBase() {}
};



template <class PT>
class Param : public ParamBase
{

public:
    Param(PT value=PT()) : _value(value), _provider(nullptr) {}
    ~Param() {} // Override to allow RTTI
    void    getValue(PT &value)  {
        // update current member value
        if (_provider)
            _provider(_value);

        // Return by reference, for generality with all types.
        value = _value;
    }
    QString &   getName()               {return &_name;}
    void        setName(QString &name)  {_name = name;}

    void        setProvider(std::function<void(PT&)> provider) {_provider = provider;}

    // ??? Should this automatically disable a _provider functor, if there is one?
    void    setValue(const PT &value) {_value = value;}


private:
    PT      _value;
    QString _name;
    std::function<void(PT &value)> _provider;
};

extern const std::type_info & paramTypeFloat;
extern const std::type_info & paramTypeInt;
extern const std::type_info & paramTypeLcolor;
#endif // PARAM_H
