#ifndef NODE_H
#define NODE_H
#include "Param.h"
#include <random>
#include <QStringList>
#include <QList>

// Abstract base class for all Nodes:
class Node
{
public:
    Node();
    virtual ~Node();

    const QString & getName() const {return _name;}
    void            setName(const QString name) {_name = name;}
    QList<ParamBase *>    getParams()        {return _paramList;}

    bool            isActive() {return _active;}
    void            setActive(bool active) {_active = active;}

    //virtual void operator() (bool &value) = 0;

protected:
    QString               _name;
    bool                  _active;
    QList<ParamBase *>    _paramList;       // Could be static
};


class TriggerEvery : public Node
{

public:
    TriggerEvery() {}

    void setTriggerInterval(int interval) {_interval = interval;}
    void operator() (bool &value);

private:
    void    reset() {_value = false;}

    bool    _value;

    // These are in milliseconds
    // Parameters
    int     _interval;
    //int     _refreshOffset; // TODO

    // Internal timekeepers
    int     _lastRefresh;
    int     _nextRefresh;
};


class RandomNode : public Node
{
public:
    RandomNode();

    // Functor that provides closure over instance object,
    // and allows downstream clients to evaluate.
    void operator() (float &value);

//private:  // XXX
    float   _value;

    // Parameters
    Param<float> _output;
    Param<float> _min;
    Param<float> _max;
//    TriggerEvery _trigger;

private:
    void  setRandomEngine();

    // Random number generator
    std::mt19937 *_randGenerator;
    std::uniform_real_distribution<float> *_distribution;
};

#endif // NODE_H
