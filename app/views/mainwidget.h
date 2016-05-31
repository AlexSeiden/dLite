#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QAudio>
#include <QWidget>
#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

#include "engine/dispatch.h"
#include "engine/nodefactory.h"
#include "engine/audioplayback.h"
#include "engine/utils.h"
#include "nodes/Node.h"
#include "views/CueLibView.h"
#include "views/GraphWidget.h"
#include "views/GuiSettings.h"
#include "views/Transport.h"
#include "views/dancefloorwidget.h"
#include "views/settingsdialog.h"
#include "views/spectrograph.h"

class FrequencySpectrum;
class Transport;
class Dancefloor;

// Main application widget, responsible for connecting the various UI
// elements to the Engine.
class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:
    void stateChanged(QAudio::State state);
    void spectrumChanged(const FrequencySpectrum &spectrum);
    void save();

    // Style: is this needed?  only for new songs--which should be a separate slot
    void bufferLengthChanged(qint64 length);

    // Style: This is more of a "model" or "controller" issue than a view/widget one.
    void newNodeRequest(QString name);

    void showLoadSongDialog();
    void showSaveDialog();
    void showOpenDialog();
    void showImportDialog();
    void showSettingsDialog();
    void updateMenuStates();
    void showAndRaise();

private:
    void createUi();
    void createMenus();
    void connectUi();
    void reset();

private:
    AudioPlayback*                 m_engine;

    // UI components
    GraphWidget *           m_graphWidget;
    Dancefloor *            m_dancefloor;
    Dancefloorwidget *      m_dancefloorwidget;
    CueLibView *            m_cueLibView;
    QDockWidget*            m_cueLibView_dw;
    Transport*              m_transport;
    QDockWidget*            m_transport_dw;
    Spectrograph*           m_spectrograph;
    QDockWidget*            m_spectrograph_dw;

    // Menu items
    QMenuBar*               m_menuBar;

    QMenu*                  m_fileMenu;
    QAction*                m_openAct;
    QAction*                m_openSongAct;
    QAction*                m_importFileAct;
    QAction*                m_saveAct;
    QAction*                m_saveAsAct;
    QAction*                m_reloadStylesAct;

    QMenu*                  m_editMenu;
    QAction*                m_cutAct;
    QAction*                m_copyAct;
    QAction*                m_pasteAct;
    QAction*                m_settingsAct;

    QMenu*                  m_controlMenu;
    QAction*                m_playAct;
    QAction*                m_pauseAct;
    QAction*                m_playPauseAct;
    QAction*                m_rewindAct;
    QAction*                m_useAllCuesAct;
    QAction*                m_autoSwitchCuesAct;

    QMenu*                  m_windowMenu;
    QAction*                m_showDancefloorwidget;
    QAction*                m_showCueLibView;
    QAction*                m_showGraphWidget;
    QAction*                m_showSpectrograph;
    QAction*                m_showTransport;

    // Dialogs
    SettingsDialog*         m_settingsDialog;

    QString                 m_filename;
};

#endif // MAINWIDGET_H
