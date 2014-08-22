#ifndef PARAM_H
#define PARAM_H

#include <functional>

#if 0
template <typename PT>
class Param
{
public:
    Param();
    const   PT& getValue();

    void    setProvider(void (*providerFunc)(PT& value));

private:
    PT      _value;
    void    (*_provider)(PT &value);
};
#endif


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



#endif // PARAM_H
