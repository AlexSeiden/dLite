// Copyright (c) 2014-2016 Alex Seiden
//
#include "views/main_widget.h"

MainWidget::MainWidget(QWidget *parent)
    :   QMainWindow(parent)
    ,   m_engine(new AudioPlayback(this))
    ,   m_graphWidget(NULL)
    ,   m_dancefloor(NULL)
    ,   m_dancefloorwidget(NULL)
    ,   m_cueLibView(NULL)
    ,   m_transport(NULL)
    ,   m_spectrograph(NULL)
    ,   m_settingsDialog(new SettingsDialog(m_engine->interval(), this))
    ,   m_filename()
{
    Dispatch::Singleton()->setEngine(m_engine);

    guisettings = new GuiSettings();
    guisettings->loadStyleSheet();

    m_dancefloor = new Dancefloor();
    Dispatch::Singleton()->setDancefloor(m_dancefloor);

    createUi();
    connectUi();

    // TODO default to last played.
    m_engine->loadSong(QString("/Users/alex/Documents/WAVS/Awesome/Awesome.wav"));

    createMenus();
    updateMenuStates();
}

MainWidget::~MainWidget() { }


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MainWidget::createUi()
{
    setWindowTitle(tr("dLite"));

    // TODO Should set default layout in preferences.
    // If I had more than one dance floor, that'd be really important!
    std::string layoutfile = std::string("/Users/alex/src/floorit/layout.csv");
    m_dancefloor->ImportLayout(layoutfile);
    RenderNode::setDancefloor(m_dancefloor);
    m_engine->setDancefloormodel(m_dancefloor);

    m_dancefloorwidget = new Dancefloorwidget();
    m_dancefloorwidget->setModel(m_dancefloor);
    m_dancefloor->setView(m_dancefloorwidget);
    Dispatch::Singleton()->setDancefloorwidget(m_dancefloorwidget);
    m_dancefloorwidget->show();

    m_cueLibView_dw = new QDockWidget(this);
    m_cueLibView = new NodeLibrary(NULL);
    m_cueLibView_dw->setWidget(m_cueLibView);
    m_cueLibView_dw->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_cueLibView_dw);

    // Spectrograph
    m_spectrograph_dw = new QDockWidget(tr("Spectrograph"), this);
    m_spectrograph = new Spectrograph(NULL);
    m_spectrograph_dw->setWidget(m_spectrograph);
    m_spectrograph_dw->setFloating(true);
    m_spectrograph_dw->move(1300, 540);
    Dispatch::Singleton()->setSpectrograph(m_spectrograph);

    // Time & Segmentation display
    m_transport_dw = new QDockWidget(tr("Transport"), this);
    m_transport = new Transport(NULL);
    m_transport_dw->setWidget(m_transport);
    m_transport_dw->setFloating(true);
    m_transport_dw->move(1300, 500);
    Dispatch::Singleton()->setTransport(m_transport);

    m_graphWidget = new GraphWidget(NULL);
    m_graphWidget->show();
    Dispatch::Singleton()->setGraphWidget(m_graphWidget);
    setCentralWidget(m_graphWidget);

    setMinimumSize(700, 400);
    resize(1500, 900);
    move(5,10);
}

void MainWidget::createMenus()
{
    // Mac-style main menu bar
    m_menuBar = new QMenuBar(nullptr);

    // ----------------------------------------
    // File actions
    m_fileMenu = m_menuBar->addMenu(tr("File"));

    m_openAct = new QAction(tr("Open dLite File..."), this);
    m_openAct->setShortcut(QKeySequence("Ctrl+O"));
    m_openAct->setShortcutContext(Qt::ApplicationShortcut);
    m_fileMenu->addAction(m_openAct);
    CHECKED_CONNECT(m_openAct, SIGNAL(triggered()), this, SLOT(showOpenDialog()));

    m_importFileAct = new QAction(tr("Import File..."), this);
    m_importFileAct->setShortcut(QKeySequence("Ctrl+I"));
    m_importFileAct->setShortcutContext(Qt::ApplicationShortcut);
    m_fileMenu->addAction(m_importFileAct);
    CHECKED_CONNECT(m_importFileAct, SIGNAL(triggered()), this, SLOT(showImportDialog()));

    m_openSongAct = new QAction(tr("Open Song..."), this);
    m_openSongAct->setShortcut(QKeySequence("Ctrl+Shift+O"));
    m_openSongAct->setShortcutContext(Qt::ApplicationShortcut);
    m_fileMenu->addAction(m_openSongAct);
    CHECKED_CONNECT(m_openSongAct, SIGNAL(triggered()), this, SLOT(showLoadSongDialog()));

    m_saveAct = new QAction(tr("Save"), this);
    m_saveAct->setShortcut(QKeySequence("Ctrl+S"));
    m_saveAct->setShortcutContext(Qt::ApplicationShortcut);
    m_fileMenu->addAction(m_saveAct);
    CHECKED_CONNECT(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAct = new QAction(tr("Save As..."), this);
    m_saveAsAct->setShortcut(QKeySequence("Ctrl+Shift+S"));
    m_saveAsAct->setShortcutContext(Qt::ApplicationShortcut);
    m_fileMenu->addAction(m_saveAsAct);
    CHECKED_CONNECT(m_saveAsAct, SIGNAL(triggered()), this, SLOT(showSaveDialog()));

    // GUI shortcuts
    m_reloadStylesAct = new QAction(tr("Reload styles"), this);
    m_reloadStylesAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    m_reloadStylesAct->setShortcutContext(Qt::ApplicationShortcut);
    m_fileMenu->addAction(m_reloadStylesAct);
    CHECKED_CONNECT(m_reloadStylesAct, SIGNAL(triggered()), guisettings, SLOT(loadStyleSheet()));

    // ----------------------------------------
    // Edit Actions
    m_editMenu = m_menuBar->addMenu(tr("Edit"));

    m_cutAct = new QAction(tr("Cut"), this);
    m_cutAct->setShortcuts(QKeySequence::Cut);
    m_cutAct->setShortcutContext(Qt::ApplicationShortcut);
    m_cutAct->setEnabled(false);
    m_editMenu->addAction(m_cutAct);

    m_copyAct = new QAction(tr("Copy"), this);
    m_copyAct->setShortcuts(QKeySequence::Copy);
    m_copyAct->setShortcutContext(Qt::ApplicationShortcut);
    m_copyAct->setEnabled(false);
    m_editMenu->addAction(m_copyAct);

    m_pasteAct = new QAction(tr("Paste"), this);
    m_pasteAct->setShortcuts(QKeySequence::Paste);
    m_pasteAct->setShortcutContext(Qt::ApplicationShortcut);
    m_pasteAct->setEnabled(false);
    m_editMenu->addAction(m_pasteAct);

    m_settingsAct = new QAction(tr("Settings..."), this);
    m_settingsAct->setShortcut(QKeySequence("Ctrl+,"));
    m_settingsAct->setShortcutContext(Qt::ApplicationShortcut);
    m_editMenu->addAction(m_settingsAct);
    CHECKED_CONNECT(m_settingsAct, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

    // ----------------------------------------
    // Control actions
    m_controlMenu = m_menuBar->addMenu(tr("Control"));

    m_playAct = new QAction(tr("Play"), this);
    m_controlMenu->addAction(m_playAct);
    CHECKED_CONNECT(m_playAct, SIGNAL(triggered()), m_engine, SLOT(startPlayback()));

    m_pauseAct = new QAction(tr("Pause"), this);
    m_controlMenu->addAction(m_pauseAct);
    CHECKED_CONNECT(m_pauseAct, SIGNAL(triggered()), m_engine, SLOT(suspend()));

    m_playPauseAct = new QAction(tr("Toggle Play/Pause"), this);
    m_playPauseAct->setShortcut(QKeySequence("Ctrl+P"));
    // XXX not really sure why this broke somewhere in my refactors...
    // m_playPauseAct->setShortcut(Qt::Key_Space);
    m_playPauseAct->setShortcutContext(Qt::ApplicationShortcut);
    m_controlMenu->addAction(m_playPauseAct);
    CHECKED_CONNECT(m_playPauseAct, SIGNAL(triggered()),
                    m_engine, SLOT(togglePlayback()));

    m_rewindAct = new QAction(tr("Rewind"), this);
    m_rewindAct->setShortcut(QKeySequence("Ctrl+0"));
    m_rewindAct->setShortcutContext(Qt::ApplicationShortcut);
    m_controlMenu->addAction(m_rewindAct);
    CHECKED_CONNECT(m_rewindAct, SIGNAL(triggered()), m_engine, SLOT(rewind()));

    m_controlMenu->addSeparator();

    m_useAllCuesAct = new QAction(tr("Use All Cues"), this);
    m_useAllCuesAct->setCheckable(true);
    m_controlMenu->addAction(m_useAllCuesAct);
    CHECKED_CONNECT(m_useAllCuesAct, SIGNAL(toggled(bool)),
                    m_dancefloor, SLOT(setUseAllCues(bool)));

    m_autoSwitchCuesAct = new QAction(tr("Auto Switch Cues"), this);
    m_autoSwitchCuesAct->setCheckable(true);
    m_controlMenu->addAction(m_autoSwitchCuesAct);
    CHECKED_CONNECT(m_autoSwitchCuesAct, SIGNAL(toggled(bool)),
                    m_graphWidget, SLOT(setAutoSwitchCues(bool)));

    // ----------------------------------------
    // Window actions
    m_windowMenu = m_menuBar->addMenu(tr("Window"));

    m_showDancefloorwidget = new QAction(tr("Dance Floor"), this);
    m_windowMenu->addAction(m_showDancefloorwidget);
    CHECKED_CONNECT(m_showDancefloorwidget, SIGNAL(triggered()),
                    m_dancefloorwidget, SLOT(showAndRaise()));

    m_showCueLibView = new QAction(tr("Node Library"), this);
    m_windowMenu->addAction(m_showCueLibView);
    CHECKED_CONNECT(m_showCueLibView, SIGNAL(triggered()),
                    m_cueLibView_dw, SLOT(show()));

    m_showGraphWidget = new QAction(tr("Graph"), this);
    m_windowMenu->addAction(m_showGraphWidget);
    CHECKED_CONNECT(m_showGraphWidget, SIGNAL(triggered()),
                    this, SLOT(showAndRaise()));

    m_showSpectrograph = new QAction(tr("Spectrograph"), this);
    m_windowMenu->addAction(m_showSpectrograph);
    CHECKED_CONNECT(m_showSpectrograph, SIGNAL(triggered()),
                    m_spectrograph_dw, SLOT(show()));

    m_showTransport = new QAction(tr("Transport"), this);
    m_windowMenu->addAction(m_showTransport);
    CHECKED_CONNECT(m_showTransport, SIGNAL(triggered()),
                    m_transport_dw, SLOT(show()));

    // TODO get cut & paste working properly for any arbitrary node selection.
#if 0
    CHECKED_CONNECT(m_cutAct, SIGNAL(triggered()), m_graphWidget, SLOT(cut()));
    CHECKED_CONNECT(m_copyAct, SIGNAL(triggered()), m_graphWidget, SLOT(copy()));
    CHECKED_CONNECT(m_pasteAct, SIGNAL(triggered()), m_graphWidget, SLOT(paste()));
    CHECKED_CONNECT(textEdit, SIGNAL(copyAvailable(bool)),
            m_cutAct, SLOT(setEnabled(bool)));
    CHECKED_CONNECT(textEdit, SIGNAL(copyAvailable(bool)),
            m_copyAct, SLOT(setEnabled(bool)));
#endif
}

void MainWidget::connectUi()
{
    CHECKED_CONNECT(m_engine, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(stateChanged(QAudio::State)));

    CHECKED_CONNECT(m_engine, SIGNAL(bufferLengthChanged(qint64)),
            this, SLOT(bufferLengthChanged(qint64)));

    // Should these go through engine/dispatch? 
    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
                    m_transport, SLOT(playPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(spectrumChanged(const FrequencySpectrum &)),
                    this, SLOT(spectrumChanged(const FrequencySpectrum &)));

    CHECKED_CONNECT(m_transport, SIGNAL(movePlaybackHead(double)),
                    m_engine, SLOT(movePlaybackHead(double)));

    CHECKED_CONNECT(m_cueLibView, SIGNAL(newNodeRequest(QString)),
                    this, SLOT(newNodeRequest(QString)));
}

void MainWidget::updateMenuStates()
{
    const bool pauseEnabled = (QAudio::ActiveState == m_engine->state() ||
                               QAudio::IdleState == m_engine->state());
    m_pauseAct->setEnabled(pauseEnabled);

    const bool playEnabled = ( (QAudio::ActiveState != m_engine->state() &&
                                QAudio::IdleState != m_engine->state()));
    m_playAct->setEnabled(playEnabled);
}

void MainWidget::reset()
{
    m_engine->reset();
    m_spectrograph->reset();
    m_transport->reset();
}

//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void MainWidget::stateChanged(QAudio::State state)
{
    updateMenuStates();

    if (QAudio::ActiveState != state && QAudio::SuspendedState != state) {
        m_spectrograph->reset();
    }
}

void MainWidget::spectrumChanged(const FrequencySpectrum &spectrum)
{
    m_spectrograph->spectrumChanged(spectrum);
}

void MainWidget::save()
{
    if (m_filename.isEmpty() || m_filename.isNull())
        showSaveDialog();
    else
        NodeFactory::Singleton()->saveToFile(m_filename);
}

void MainWidget::bufferLengthChanged(qint64 length)
{
    m_transport->bufferLengthChanged(length);
}

void MainWidget::showAndRaise()
{
    show();
    raise();
}

//-----------------------------------------------------------------------------
// New node slots
//-----------------------------------------------------------------------------
// TODO: This probably should be in Dispatch or Nodefactory
void MainWidget::newNodeRequest(QString name)
{
    // TODO generate this list from a single place, where all cues are listed,
    // and the same place is used for the CueLib. Move to cuelibview.cpp
    Node *newNode;

    newNode = NodeFactory::Singleton()->instatiateNode(name);
    if (newNode)
        m_graphWidget->addNode(newNode);
    else {
        qDebug() << "Unknown newNodeRequest for " << name;
    }
}
