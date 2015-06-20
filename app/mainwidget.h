#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QAudio>
#include <QIcon>
#include <QWidget>
#include <QIntValidator>
#include <QSpinBox>
#include <QRubberBand>
#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>

#include "engine.h"
#include "NodeFactory.h"
#include "settingsdialog.h"
#include "Transport.h"
#include "spectrograph.h"
#include "utils.h"
#include "dancefloorwidget.h"
#include "CueLibView.h"
#include "GraphWidget.h"
#include "Node.h"
#include "Cupid.h"
#include "GuiSettings.h"

class FrequencySpectrum;
class Transport;
class Dancefloor;

QT_BEGIN_NAMESPACE
class QAction;
class QAudioFormat;
class QLabel;
class QMenu;
class QPushButton;
class QSpinBox;
class QLineEdit;
class QShortcut;
class QMenuBar;
QT_END_NAMESPACE

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
    void spectrumChanged(qint64 position, qint64 length,
                         const FrequencySpectrum &spectrum);
    void save();

    // XXX is this needed?  only for new songs--which should be a separate slot
    void bufferLengthChanged(qint64 length);

    // Style: This is more of a "model" or "controller" issue than a view/widget one.
    void newNodeRequest(QString name);

    void showLoadSongDialog();
    void showSaveDialog();
    void showOpenDialog();
    void showImportDialog();
    void showSettingsDialog();
    void updateMenuStates();

private:
    void createUi();
    void createShortcuts();
    void createActions();
    void connectUi();
    void reset();

private:
    Engine*                 m_engine;

    Transport*              m_transport;
    Spectrograph*           m_spectrograph;

    QShortcut*              m_importFileShortcut;
    QShortcut*              m_openSongShortcut;
    QShortcut*              m_reloadStylesShortcut;

    // Menu items
    QMenuBar*               m_menuBar;

    QMenu*                  m_fileMenu;
    QAction*                m_openAct;
    QAction*                m_openSongAct;
    QAction*                m_importFileAct;
    QAction*                m_saveAct;
    QAction*                m_saveAsAct;

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

    QMenu*                  m_windowMenu;
    QAction*                m_showDancefloorwidget;
    QAction*                m_showCueLibView;
    QAction*                m_showGraphWidget;

    // Dialogs
    SettingsDialog*         m_settingsDialog;
    Dancefloor *            m_dancefloor;
    Dancefloorwidget *      m_dancefloorwidget;
    CueLibView *            m_cueLibView;
    GraphWidget *           m_graphWidget;

    QString                 m_filename;
};

#endif // MAINWIDGET_H
