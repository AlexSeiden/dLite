#include "Param.h"

#if 1
//template <class PT>
//Param<PT>::Param() :
//    _value(PT()),
//    _provider(nullptr)
//{
//}

#else
ParamFloat::ParamFloat(float value) :
    _value(value),
    _provider(nullptr)
{
}

void
ParamFloat::getValue(float &value)
{
    // update current member value
    if (_provider)
        _provider(_value);

    // Return by reference, for generality with other types.
    value = _value;
}


ParamInt::ParamInt(int value) :
    _value(value),
    _provider(nullptr)
{
}

void
ParamInt::getValue(int &value)
{
    if (!_provider)
        value = _value;
    else
        _provider(value);
}


void
ParamFloat::setProvider(providerFunctor_t &provider)
{
    _provider = provider;
}


void
ParamInt::setProvider(void (*provider)(int &value))
{
    _provider = provider;
}

// should have destructor destroy provider? or ref count???
#endif


