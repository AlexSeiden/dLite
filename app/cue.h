#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "dancefloormodel.h"
#include "Param.h"
#include <random>

// Need forward class declaration, since both Cue.h and dancefloormodel.h include each other
class Dancefloormodel;

class Cue
{
public:
    explicit Cue(Dancefloormodel *dfmodel = 0);

    void            setCompMode(compmode_t mode);
    compmode_t      getCompMode();

    const QString & getName() const {return _name;}
    void            setName(const QString& name) {_name = name;}

    virtual void evaluate();

protected:
    Dancefloormodel *_dfModel;
    bool            _active;
    compmode_t      _compmode;      // TODO implement compmodes
    int             _decaymode;     // TODO

    QString         _name;
};

class TriggerEvery
{
public:
    TriggerEvery() {}

    void setTriggerInterval(int interval) {_interval = interval;}
    void operator() (bool &value);

    void    reset() {_value = false;}

private:
    bool    _value;
    // These are in milliseconds
    int     _interval;
    int     _lastRefresh;
    int     _nextRefresh;
    int     _refreshOffset; // TODO
};


class RandomNode
{
public:
    RandomNode();

    void operator() (float &value);

private:
    float   _value;
    float   _min;
    float   _max;
    TriggerEvery _trigger;

    void  setRandomEngine();

    // Random number generator
    std::mt19937 *_randGenerator;
    std::uniform_real_distribution<float> *_distribution;
};

#endif // CUE_H
