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
//      You could call it an "observer pattern", but that's really
//      just a fancy way to say "global variables."
class Cupid : public QObject
{
    Q_OBJECT

public:
    explicit Cupid(QObject *parent = 0);

    // ??? should override new?
    static Cupid *Singleton();

    QObject*        getSpectrograph()                       {return m_spectrograph;}
    void            setSpectrograph(QObject* spectrograph)  {m_spectrograph = spectrograph;}
    Dancefloor*     getDancefloor()                         {return m_dancefloor;}
    void            setDancefloor(Dancefloor* dfmodel)      {m_dancefloor = dfmodel;}
    Engine*         getEngine()                     {return m_engine;}
    void            setEngine(Engine* engine)       {m_engine = engine;}
    GraphWidget*    getGraphWidget()                {return m_graphWidget;}
    void            setGraphWidget(GraphWidget* gw) {m_graphWidget = gw;}
    Transport*      getTransport()                  {return m_transport;}
    void            setTransport(Transport* t)      {m_transport = t;}
    Dancefloorwidget*     getDancefloorwidget()                     {return m_dancefloorwidget;}
    void            setDancefloorwidget(Dancefloorwidget* dfwidget) {m_dancefloorwidget = dfwidget;}

    // Syntactic sugar
    static int      getCurrentFrame()
                        {return Singleton()->m_dancefloor->getFrame();}
    static int      getPlaybackPositionUSecs()
                        {return Singleton()->m_engine->getCurrentTime();}
    static QString  getAudioFilename()
                        {return Singleton()->m_engine->getAudioFilename();}

signals:
    // Forward to engine (via mainwidget for now TODO)
    void togglePlayback();
    void rewind();
    void suspend();
    void startPlayback();

    // Forward to MainWindow
    void showLoadSongDialog();
    void showSaveDialog();
    void showOpenDialog();
    void showSettingsDialog();
    void showImportDialog();

    // Forward to TransportControl
    void updateButtonStates();

private:
    // Display widgets
    QObject*                m_spectrograph;
    GraphWidget*            m_graphWidget;
    Dancefloorwidget*       m_dancefloorwidget;
    Transport*              m_transport;

    Dancefloor*             m_dancefloor;
    Engine*                 m_engine;
};

#endif // CUPID_H
