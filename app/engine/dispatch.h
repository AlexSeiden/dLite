#ifndef DISPATCH_H
#define DISPATCH_H

#include <QObject>
#include "engine/nodeparameters.h"
#include "engine/dancefloor.h"
#include "engine/audioplayback.h"

class GraphWidget;
class Dancefloorwidget;
class Transport;

// Dispatch
//      How things get hooked together.
//      You could call it an "observer pattern".
class Dispatch : public QObject
{
    Q_OBJECT

public:
    explicit Dispatch(QObject *parent = 0);
    static Dispatch *Singleton();

    QObject*        getSpectrograph()                       {return m_spectrograph;}
    void            setSpectrograph(QObject* spectrograph)  {m_spectrograph = spectrograph;}
    Dancefloor*     getDancefloor()                         {return m_dancefloor;}
    void            setDancefloor(Dancefloor* dfmodel)      {m_dancefloor = dfmodel;}
    AudioPlayback*         getEngine()                     {return m_engine;}
    void            setEngine(AudioPlayback* engine)       {m_engine = engine;}
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
    // Forward to engine (TODO: these go to mainwidget now, but should go directly)
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
    AudioPlayback*                 m_engine;
};

#endif // DISPATCH_H
