#ifndef OKCUPID_H
#define OKCUPID_H

#include <QObject>
#include "Param.h"

class Dancefloormodel;
class Engine;

class OKCupid : public QObject
{
    Q_OBJECT


public:
    explicit OKCupid(QObject *parent = 0);
    // ??? should override new?
    static OKCupid *Singleton();

signals:

public slots:
    void    inputSeeksOutput(ParamBase *param);
    void    outputSeeksInput(ParamBase *param);

    QObject * getSpectrograph() { return _spectrograph;}
    void  setSpectrograph(QObject *spectrograph) {_spectrograph = spectrograph;}
    Dancefloormodel * getDancefloormodel() { return _dancefloormodel;}
    void  setDancefloormodel(Dancefloormodel *dfmodel) {_dancefloormodel = dfmodel;}
    Engine * getEngine() { return _engine;}
    void  setEngine(Engine *engine) {_engine = engine;}

private:
    QObject *_spectrograph;
    Dancefloormodel *_dancefloormodel;
    Engine *_engine;
};

#endif // OKCUPID_H
