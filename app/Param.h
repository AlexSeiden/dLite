#ifndef PARAM_H
#define PARAM_H

#include <functional>

#if 1
template <class PT>
class Param
{
public:
    Param(PT value=PT()) : _value(value), _provider(nullptr) {}
    void    getValue(PT &value)  {
        // update current member value
        if (_provider)
            _provider(_value);

        // Return by reference, for generality with other types.
        value = _value;
    }
    void    setValue(const PT &value) {_value = value;}
    void    setProvider(std::function<void(PT&)> provider) {_provider = provider;}

private:
    PT      _value;
    std::function<void(PT &value)> _provider;
};

#else

typedef std::function<void(float&)> providerFunctor_t;

class ParamFloat
{
public:
    ParamFloat(float value=0.0);
    void getValue(float &value);

    void setProvider(providerFunctor_t &provider);

private:
    float _value;
    providerFunctor_t _provider;
};


class ParamInt
{
public:
    ParamInt(int value=0);
    void getValue(int &value);
    
    void setProvider(void (*provider)(int &value));
    
private:
    int _value;
    void (*_provider)(int &value);
};

#endif


#endif // PARAM_H
