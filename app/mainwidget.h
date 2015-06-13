#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QAudio>
#include <QIcon>
#include <QWidget>
#include <QIntValidator>
#include <QSpinBox>
#include <QRubberBand>
#include <QApplication>

class Engine;
class FrequencySpectrum;
class SublevelMeter;
class Transport;
class SettingsDialog;
class Spectrograph;
class Dancefloorwidget;
class Dancefloor;
class CueLibView;
class GraphWidget;

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
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

    void timerEvent(QTimerEvent *event);

public slots:
    void stateChanged(QAudio::State state);
    void spectrumChanged(qint64 position, qint64 length,
                         const FrequencySpectrum &spectrum);
#if 0  // NUKEMEMAYBE
    void infoMessage(const QString &message, int timeoutMs);
    void errorMessage(const QString &heading, const QString &detail);
#endif

    void save();
    void showSaveDialog();
    void loadStyleSheet();

    // XXX is this still needed?
    void audioPositionChanged(qint64 position);

    // XXX is this needed?  only for new songs--which should be a separate slot
    void bufferLengthChanged(qint64 length);

    // Style: This is more of a "model" or "controller" issue than a view/widget one.
    void newNodeRequest(QString name);

private slots:
    void showLoadSongDialog();
    void showOpenDialog();
    void showImportDialog();
    void showSettingsDialog();
    void updateButtonStates();

private:
    void createUi();
    void createShortcuts();
    void createActions();
    void createMenus();
    void connectUi();
    void reset();

private:
    Engine*                 m_engine;

    Transport*              m_transport;
    Spectrograph*           m_spectrograph;

    // Atomic UI items for the controllerwindow
    QPushButton*            m_fileButton;
    QPushButton*            m_saveButton;
    QPushButton*            m_openButton;
    QPushButton*            m_pauseButton;
    QIcon                   m_pauseIcon;
    QPushButton*            m_playButton;
    QIcon                   m_playIcon;
    QPushButton*            m_settingsButton;
    QIcon                   m_settingsIcon;

    QShortcut*              m_playPauseShortcut;
    QShortcut*              m_rewindShortcut;
    QShortcut*              m_frameAllShortcut;
    QShortcut*              m_frameSelectedShortcut;
    QShortcut*              m_layoutAllShortcut;
    QShortcut*              m_xAlignShortcut;
    QShortcut*              m_yAlignShortcut;
    QShortcut*              m_xDistributeShortcut;
    QShortcut*              m_yDistributeShortcut;
    QShortcut*              m_zoomOutShortcut;
    QShortcut*              m_zoomInShortcut;
    QShortcut*              m_zoomResetShortcut;
    QShortcut*              m_saveShortcut;
    QShortcut*              m_saveAsShortcut;
    QShortcut*              m_openFileShortcut;
    QShortcut*              m_importFileShortcut;
    QShortcut*              m_openSongShortcut;
    QShortcut*              m_duplicateShortcut;
    QShortcut*              m_groupShortcut;
    QShortcut*              m_ungroupShortcut;
    QShortcut*              m_minimizeSelectedShortcut;
    QShortcut*              m_newTabShortcut;
    QList<QShortcut*>       m_cueShortcut;

    QMenuBar*               m_menuBar;
    QMenu*                  m_fileMenu;
    QMenu*                  m_editMenu;
    QMenu*                  m_windowMenu;
    QAction*                m_newAct;
    QAction*                m_openAct;
    QAction*                m_saveAct;
    QAction*                m_saveAsAct;
    QAction*                m_exitAct;
    QAction*                m_cutAct;
    QAction*                m_copyAct;
    QAction*                m_pasteAct;
    QAction*                m_showDancefloorwidget;
    QAction*                m_showCueLibView;
    QAction*                m_showGraphWidget;

#if 1 // NUKEMEMAYBE
    QLabel*                 m_infoMessage;
    int                     m_infoMessageTimerId;
#endif

    SettingsDialog*         m_settingsDialog;

    Dancefloor *            m_dancefloor;
    Dancefloorwidget *      m_dancefloorwidget;
    CueLibView *            m_cueLibView;
    GraphWidget *           m_graphWidget;

    QString                 _filename;
};

#endif // MAINWIDGET_H
