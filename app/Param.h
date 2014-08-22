#ifndef PARAM_H
#define PARAM_H

#include <functional>

template <class PT>
class Param
{
public:
    Param(PT value=PT()) : _value(value), _provider(nullptr) {}
    void    getValue(PT &value)  {
        // update current member value
        if (_provider)
            _provider(_value);

        // Return by reference, for generality with all types.
        value = _value;
    }
    void    setValue(const PT &value) {_value = value;}
    void    setProvider(std::function<void(PT&)> provider) {_provider = provider;}

private:
    PT      _value;
    std::function<void(PT &value)> _provider;
};

#endif // PARAM_H
