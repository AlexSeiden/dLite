#ifndef CUPID_H
#define CUPID_H

#include <QObject>
#include "Param.h"
#include "dancefloormodel.h"
#include "engine.h"

class GraphWidget;

// Cupid
//      How things get hooked together.
//      A fancy way of doing global variables.  GROSS
class Cupid : public QObject
{
    Q_OBJECT

public:
    explicit Cupid(QObject *parent = 0);

    // ??? should override new?
    static Cupid *Singleton();

    QObject*        getSpectrograph()
                        { return _spectrograph;}
    void            setSpectrograph(QObject* spectrograph)
                        {_spectrograph = spectrograph;}
    Dancefloormodel* getDancefloormodel()
                        { return _dancefloormodel;}
    void            setDancefloormodel(Dancefloormodel* dfmodel)
                        {_dancefloormodel = dfmodel;}
    Engine*         getEngine()
                        { return _engine;}
    void            setEngine(Engine* engine)
                        {_engine = engine;}
    GraphWidget*    getGraphWidget()
                        { return _graphWidget;}
    void            setGraphWidget(GraphWidget* gw)
                        {_graphWidget = gw;}

    // Syntactic sugar
    static int      getCurrentFrame()
                        {return Singleton()->_dancefloormodel->getFrame();}
    static int      getPlaybackPositionUSecs()
                        {return Singleton()->_engine->_uSecs;}
    static QString  getAudioFilename()
                        {return Singleton()->_engine->m_audiofilename;}

#if 0 // NUKEMEMAYBE
public slots:
    void    inputSeeksOutput(ParamBase *param);
    void    outputSeeksInput(ParamBase *param);
#endif

private:
    QObject*         _spectrograph;
    Dancefloormodel* _dancefloormodel;
    Engine*          _engine;
    GraphWidget*     _graphWidget;
};

#endif // CUPID_H
