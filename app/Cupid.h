#ifndef CUPID_H
#define CUPID_H

#include <QObject>
#include "Param.h"
#include "DanceFloor.h"
#include "engine.h"

class GraphWidget;
class Dancefloorwidget;
class Transport;

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

    QObject*        getSpectrograph()                       {return _spectrograph;}
    void            setSpectrograph(QObject* spectrograph)  {_spectrograph = spectrograph;}
    Dancefloor*     getDancefloor()                         {return _dancefloor;}
    void            setDancefloor(Dancefloor* dfmodel)      {_dancefloor = dfmodel;}
    Engine*         getEngine()                     {return _engine;}
    void            setEngine(Engine* engine)       {_engine = engine;}
    GraphWidget*    getGraphWidget()                {return _graphWidget;}
    void            setGraphWidget(GraphWidget* gw) {_graphWidget = gw;}
    Transport*      getTransport()                  {return _transport;}
    void            setTransport(Transport* t)      {_transport = t;}
    Dancefloorwidget*     getDancefloorwidget()                     {return _dancefloorwidget;}
    void            setDancefloorwidget(Dancefloorwidget* dfwidget) {_dancefloorwidget = dfwidget;}

    // Syntactic sugar
    static int      getCurrentFrame()
                        {return Singleton()->_dancefloor->getFrame();}
    static int      getPlaybackPositionUSecs()
                        {return Singleton()->_engine->getCurrentTime();}
    static QString  getAudioFilename()
                        {return Singleton()->_engine->getAudioFilename();}

private:
    // Display widgets
    QObject*                _spectrograph;
    GraphWidget*            _graphWidget;
    Dancefloorwidget*       _dancefloorwidget;
    Transport*              _transport;

    Dancefloor*             _dancefloor;
    Engine*                 _engine;
};

#endif // CUPID_H
