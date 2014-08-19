#ifndef PARAM_H
#define PARAM_H

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

class ParamFloat
{
public:
    ParamFloat(float value=0.0);
    void getValue(float &value);

    void setProvider();

private:
    float _value;
    void (*_provider)(float &value);
};


class ParamInt
{
public:
    ParamInt(int value=0);
    void getValue(int &value);
    
    void setProvider();
    
private:
    int _value;
    void (*_provider)(int &value);
};

#endif // PARAM_H
