#include "Param.h"
//#include <QVariant>

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

// should have destructor destroy provider?


