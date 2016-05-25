#ifndef CUE_H
#define CUE_H

#include "lightcolor.h"
#include "Node.h"
#include "Firing.h"

class Dancefloor;

class Cue : public Node
{
public:
    Cue();
    ~Cue();

    void            setCompMode(compMode_t mode)   {m_compMode = mode;}
    compMode_t      getCompMode()                  {return m_compMode;}
    void            setDecayMode(decayMode_t mode) {m_decayMode = mode;}
    decayMode_t     getDecayMode()                 {return m_decayMode;}

    virtual void    evaluate() = 0;
    virtual Cue*    clone() = 0;

    static void     setDancefloor(Dancefloor *dfmodel) {m_dfModel = dfmodel;}

    void writeToJSONObj(QJsonObject &json) const;
    void readFromJSONObj(const QJsonObject &json);

protected:
    compMode_t      m_compMode;
    decayMode_t     m_decayMode;

    // Having this be static means we don't have to set it for every cue,
    // but obviously limits us to one dfmodel.  This probably isn't an issue;
    // and having to init every cue with the dfmodel would have made the
    // instatiation code in CueLibView.cpp and/or Node.cpp more complicated.
    // And right now, I only have one dance floor at a time. :-)
    static Dancefloor *m_dfModel;
};

#endif // CUE_H
