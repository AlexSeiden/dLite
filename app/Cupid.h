#ifndef CUPID_H
#define CUPID_H

#include <QObject>
#include "Param.h"
#include "dancefloormodel.h"
#include "engine.h"

// Cupid
//      How things get hooked together.
//      A fancy way of doing global variables.  GROSS, I guess.
class Cupid : public QObject
{
    Q_OBJECT

public:
    explicit Cupid(QObject *parent = 0);

    // ??? should override new?
    static Cupid *Singleton();

    QObject *       getSpectrograph()
                        { return _spectrograph;}
    void            setSpectrograph(QObject *spectrograph)
                        {_spectrograph = spectrograph;}
    Dancefloormodel * getDancefloormodel()
                        { return _dancefloormodel;}
    void            setDancefloormodel(Dancefloormodel *dfmodel)
                        {_dancefloormodel = dfmodel;}
    Engine *        getEngine()
                        { return _engine;}
    void            setEngine(Engine *engine)
                        {_engine = engine;}

    // Syntactic sugar
    static int      getCurrentFrame()
                        {return Singleton()->_dancefloormodel->_frame;}
    static int      getPlaybackPositionUSecs()
                        {return Singleton()->_engine->_uSecs;}
    static QString  getAudioFilename()
                        {return Singleton()->_engine->m_audiofilename;}

public slots:
    void    inputSeeksOutput(ParamBase *param);
    void    outputSeeksInput(ParamBase *param);

private:
    QObject *       _spectrograph;
    Dancefloormodel *_dancefloormodel;
    Engine *        _engine;
};

#endif // CUPID_H
