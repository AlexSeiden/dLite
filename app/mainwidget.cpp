#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    :   QMainWindow(parent)
    ,   m_engine(new Engine(this))
    ,   m_transport(new Transport(this))
    ,   m_spectrograph(new Spectrograph(this))
    ,   m_settingsDialog(new SettingsDialog(m_engine->interval(), this))
    ,   m_dancefloor(NULL)
    ,   m_cueLibView(NULL)
    ,   m_filename()
{
    guisettings = new GuiSettings();
    guisettings->loadStyleSheet();
    m_dancefloor = new Dancefloor();
    Cupid::Singleton()->setEngine(m_engine);
    Cupid::Singleton()->setDancefloor(m_dancefloor);
    Cupid::Singleton()->setSpectrograph(m_spectrograph);
    Cupid::Singleton()->setTransport(m_transport);
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

    // These cause problems with the dock widgets
//    setMinimumSize(500, 400);
//    resize(600, 450);
//    move(5,10);  // TODO restore from last saved

    // TODO move to settings/prefs  & allow setting this
    std::string layoutfile = std::string("/Users/alex/src/floorit/layout.csv");
    m_dancefloor->ImportLayout(layoutfile);
    Cue::setDancefloor(m_dancefloor);
    m_engine->setDancefloormodel(m_dancefloor);

    m_dancefloorwidget = new Dancefloorwidget();
    m_dancefloorwidget->setModel(m_dancefloor);
    m_dancefloor->setView(m_dancefloorwidget);  // GROSS
    Cupid::Singleton()->setDancefloorwidget(m_dancefloorwidget);
    m_dancefloorwidget->show();

    m_cueLibView = new CueLibView(NULL);
    m_cueLibView->show();

    m_graphWidget = new GraphWidget(NULL);
    m_graphWidget->show();
    Cupid::Singleton()->setGraphWidget(m_graphWidget);
    setCentralWidget(m_graphWidget);

    // Spectrograph
    QScopedPointer<QDockWidget> spectrograph_dw(new QDockWidget(tr("Spectrograph"), this));
    spectrograph_dw->setWidget(m_spectrograph);
    spectrograph_dw->setFloating(true);
    spectrograph_dw.take();
}

void MainWidget::createMenus()
{
    // Mac-style main menu bar
    m_menuBar = new QMenuBar(nullptr);

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

    // Control actions
    m_controlMenu = m_menuBar->addMenu(tr("Control"));

    m_playAct = new QAction(tr("Play"), this);
    m_controlMenu->addAction(m_playAct);
    CHECKED_CONNECT(m_playAct, SIGNAL(triggered()), m_engine, SLOT(startPlayback()));

    m_pauseAct = new QAction(tr("Pause"), this);
    m_controlMenu->addAction(m_pauseAct);
    CHECKED_CONNECT(m_pauseAct, SIGNAL(triggered()), m_engine, SLOT(suspend()));

    m_playPauseAct = new QAction(tr("Toggle Play/Pause"), this);
    m_playPauseAct->setShortcut(Qt::Key_Space);
    m_playPauseAct->setShortcutContext(Qt::ApplicationShortcut);
    m_controlMenu->addAction(m_playPauseAct);
    CHECKED_CONNECT(m_playPauseAct, SIGNAL(triggered()), m_engine, SLOT(togglePlayback()));

    m_rewindAct = new QAction(tr("Rewind"), this);
    m_rewindAct->setShortcut(QKeySequence("Ctrl+0"));
    m_rewindAct->setShortcutContext(Qt::ApplicationShortcut);
    m_controlMenu->addAction(m_rewindAct);
    CHECKED_CONNECT(m_rewindAct, SIGNAL(triggered()), m_engine, SLOT(rewind()));

    // ----------------------------------------
    // Window actions
    m_windowMenu = m_menuBar->addMenu(tr("Window"));

    m_showDancefloorwidget = new QAction(tr("Dance Floor"), this);
    m_windowMenu->addAction(m_showDancefloorwidget);
    CHECKED_CONNECT(m_showDancefloorwidget, SIGNAL(triggered()), m_dancefloorwidget, SLOT(showAndRaise()));

    m_showCueLibView = new QAction(tr("Node Library"), this);
    m_windowMenu->addAction(m_showCueLibView);
    CHECKED_CONNECT(m_showCueLibView, SIGNAL(triggered()), m_cueLibView, SLOT(showAndRaise()));

    m_showGraphWidget = new QAction(tr("Graph"), this);
    m_windowMenu->addAction(m_showGraphWidget);
    CHECKED_CONNECT(m_showGraphWidget, SIGNAL(triggered()), m_graphWidget, SLOT(show()));

    // TODO get cut & paste working
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

    // TODO There should be a "song changed" signal/slot

    // Should these go through cupid? TODO
    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
                    m_transport, SLOT(playPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
                    this, SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));

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

// TODO decouple position & spectrum change
// They are artificially coupled in engine.cpp
void MainWidget::spectrumChanged(qint64 position, qint64 length,
                                 const FrequencySpectrum &spectrum)
{
    Q_UNUSED(position)
    Q_UNUSED(length)
    // ??? Why is this a function call, instead of a signal?
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

//-----------------------------------------------------------------------------
// New node slots
//-----------------------------------------------------------------------------
// GROSS : move this to Cupid?  Or Nodefactory? Or somewhere else?
void MainWidget::newNodeRequest(QString name)
{
    // TODO generate this list from a single place, where all cues are listed,
    // and the same place is used for the CueLib
    // move to cuelibview.cpp
    Node *newNode;

    newNode = NodeFactory::Singleton()->instatiateNode(name);
    if (newNode)
        // Would "correct MVC" mean that the graphWidget should figure this out
        // on its own, by talking to the "model"?
        // This is also GROSS
        m_graphWidget->addNode(newNode);
    else {
        qDebug() << "Unknown newNodeRequest for " << name;
        // ErrorHandling
    }
}
