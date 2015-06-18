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
#include "TransportControl.h"

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

    // XXX is this still needed?
    void audioPositionChanged(qint64 position);

    // XXX is this needed?  only for new songs--which should be a separate slot
    void bufferLengthChanged(qint64 length);

    // Style: This is more of a "model" or "controller" issue than a view/widget one.
    void newNodeRequest(QString name);

    void showLoadSongDialog();
    void showSaveDialog();
    void showOpenDialog();
    void showImportDialog();
    void showSettingsDialog();

signals:
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
    TransportControl*       m_transportControl;
    Spectrograph*           m_spectrograph;

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
    QShortcut*              m_reloadStylesShortcut;
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

    SettingsDialog*         m_settingsDialog;

    Dancefloor *            m_dancefloor;
    Dancefloorwidget *      m_dancefloorwidget;
    CueLibView *            m_cueLibView;
    GraphWidget *           m_graphWidget;

    QString                 m_filename;
};

#endif // MAINWIDGET_H
