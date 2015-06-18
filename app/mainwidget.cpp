#include "mainwidget.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QFileDialog>
#include <QTimerEvent>
#include <QMessageBox>
#include <QShortCut>
#include <QMenuBar>

MainWidget::MainWidget(QWidget *parent)
    :   QMainWindow(parent)
    ,   m_engine(new Engine(this))
    ,   m_transport(new Transport(this))
    ,   m_spectrograph(new Spectrograph(this))
    ,   m_settingsDialog(new SettingsDialog(m_engine->interval(), this))
    ,   m_dancefloor(new Dancefloor)
    ,   m_cueLibView(NULL)
    ,   m_filename()
{
    // GROSS  These are essentially globals:
    Cupid::Singleton()->setEngine(m_engine);
    Cupid::Singleton()->setDancefloor(m_dancefloor);
    Cupid::Singleton()->setSpectrograph(m_spectrograph);
    Cupid::Singleton()->setTransport(m_transport);
    guisettings = new GuiSettings();
    guisettings->loadStyleSheet();

    createUi();

    // These cause problems with the dock widgets
//    setMinimumSize(500, 400);
//    resize(600, 450);
    move(5,10);  // TODO restore from last saved

    // TODO move to settings/prefs  & allow setting this
    std::string layoutfile = std::string("/Users/alex/src/floorit/layout.csv");
    m_dancefloor->ImportLayout(layoutfile);
    Cue::setDancefloor(m_dancefloor);
    m_engine->setDancefloormodel(m_dancefloor);

    m_dancefloorwidget = new Dancefloorwidget();
    m_dancefloorwidget->setModel(m_dancefloor);
    m_dancefloor->setView(m_dancefloorwidget);  // GROSS
    setCentralWidget(m_dancefloorwidget);
    Cupid::Singleton()->setDancefloorwidget(m_dancefloorwidget);

    m_cueLibView = new CueLibView(NULL);
    m_cueLibView->show();

    m_graphWidget = new GraphWidget(NULL);
    m_graphWidget->show();
    Cupid::Singleton()->setGraphWidget(m_graphWidget);

    connectUi();

    // TODO default to last played.
    m_engine->loadSong(QString("/Users/alex/Documents/WAVS/Awesome/Awesome.wav"));
    emit updateButtonStates();

    createShortcuts();
    createActions();
    createMenus();
}

MainWidget::~MainWidget() { }


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MainWidget::createUi()
{
    setWindowTitle(tr("dLite"));

    m_transportControl = new TransportControl(this);
    addToolBar(Qt::BottomToolBarArea, m_transportControl);

#if 1
    QToolBar *transport_tb = addToolBar(tr("Transport"));
    QAction *transport_act = transport_tb->addWidget(m_transport);
    transport_act->setVisible(true);


#else
    QDockWidget *transport_dw = new QDockWidget(this);
    transport_dw->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    transport_dw->setWidget(m_transport);
    addDockWidget(Qt::TopDockWidgetArea, transport_dw);
#endif

    // Spectrograph
    QScopedPointer<QDockWidget> spectrograph_dw(new QDockWidget(tr("Spectrograph"), this));
    spectrograph_dw->setWidget(m_spectrograph);
    spectrograph_dw->setFloating(true);
    spectrograph_dw.take();
}

void MainWidget::createActions()
{
#if 0
    m_newAct = new QAction(tr("New"), this);
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
#endif

    m_openAct = new QAction(tr("Open..."), this);
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(showOpenDialog()));

    m_saveAct = new QAction(tr("Save"), this);
    connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAct = new QAction(tr("Save As..."), this);
    connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(showSaveDialog()));

    m_showDancefloorwidget = new QAction(tr("Dance Floor"), this);
    connect(m_showDancefloorwidget, SIGNAL(triggered()), m_dancefloorwidget, SLOT(show()));

    m_showGraphWidget = new QAction(tr("Graph"), this);
    connect(m_showGraphWidget, SIGNAL(triggered()), m_graphWidget, SLOT(show()));

    m_showCueLibView = new QAction(tr("Node Library"), this);
    connect(m_showCueLibView, SIGNAL(triggered()), m_cueLibView, SLOT(show()));


    m_cutAct = new QAction(tr("Cut"), this);
    m_cutAct->setShortcuts(QKeySequence::Cut);
//    connect(m_cutAct, SIGNAL(triggered()), m_graphWidget, SLOT(cut()));

    m_copyAct = new QAction(tr("Copy"), this);
    m_copyAct->setShortcuts(QKeySequence::Copy);
//    connect(m_copyAct, SIGNAL(triggered()), m_graphWidget, SLOT(copy()));

    m_pasteAct = new QAction(tr("Paste"), this);
    m_pasteAct->setShortcuts(QKeySequence::Paste);
//    connect(m_pasteAct, SIGNAL(triggered()), m_graphWidget, SLOT(paste()));

    m_cutAct->setEnabled(false);
    m_copyAct->setEnabled(false);
#if 0
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            m_cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            m_copyAct, SLOT(setEnabled(bool)));
#endif
}

void MainWidget::createMenus()
{
    // Mac-style main menu bar
    m_menuBar = new QMenuBar(nullptr);
    m_fileMenu = m_menuBar->addMenu(tr("File"));
//    m_fileMenu->addAction(m_newAct);
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_saveAct);
    m_fileMenu->addAction(m_saveAsAct);

    m_editMenu = m_menuBar->addMenu(tr("Edit"));
    m_editMenu->addAction(m_cutAct);
    m_editMenu->addAction(m_copyAct);
    m_editMenu->addAction(m_pasteAct);

    m_windowMenu = m_menuBar->addMenu(tr("Window"));
    m_windowMenu->addAction(m_showDancefloorwidget);
    m_windowMenu->addAction(m_showGraphWidget);
    m_windowMenu->addAction(m_showCueLibView);
}

void MainWidget::connectUi()
{
    CHECKED_CONNECT(m_engine, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(stateChanged(QAudio::State)));

    CHECKED_CONNECT(m_engine, SIGNAL(bufferLengthChanged(qint64)),
            this, SLOT(bufferLengthChanged(qint64)));

    // TODO There should be a "song changed" signal/slot

    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
            m_transport, SLOT(playPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
            this, SLOT(audioPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
            this, SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));

    CHECKED_CONNECT(m_transport, SIGNAL(movePlaybackHead(double)),
            m_engine, SLOT(movePlaybackHead(double)));

    CHECKED_CONNECT(m_cueLibView, SIGNAL(newNodeRequest(QString)),
                    this, SLOT(newNodeRequest(QString)));
}

void MainWidget::createShortcuts()
{
    // ----------------------------------------
    // Graph view shortcuts
    m_frameAllShortcut = new QShortcut(Qt::Key_A, this);
    m_frameAllShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_frameAllShortcut, SIGNAL(activated()), m_graphWidget, SLOT(frameAll()));

    m_frameSelectedShortcut = new QShortcut(Qt::Key_F, this);
    m_frameSelectedShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_frameSelectedShortcut, SIGNAL(activated()), m_graphWidget, SLOT(frameSelection()));

    m_zoomOutShortcut = new QShortcut(Qt::Key_Minus, this);
    m_zoomOutShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_zoomOutShortcut, SIGNAL(activated()), m_graphWidget, SLOT(zoomOut()));

    m_zoomInShortcut = new QShortcut(Qt::Key_Equal, this);
    m_zoomInShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_zoomInShortcut, SIGNAL(activated()), m_graphWidget, SLOT(zoomIn()));

    m_zoomResetShortcut = new QShortcut(Qt::Key_BracketLeft, this);
    m_zoomResetShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_zoomResetShortcut, SIGNAL(activated()), m_graphWidget, SLOT(zoomReset()));

    m_layoutAllShortcut = new QShortcut(Qt::Key_L, this);
    m_layoutAllShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_layoutAllShortcut, SIGNAL(activated()), m_graphWidget, SLOT(layoutAll()));

    m_xAlignShortcut = new QShortcut(Qt::Key_X, this);
    m_xAlignShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_xAlignShortcut, SIGNAL(activated()), m_graphWidget, SLOT(xAlign()));

    m_yAlignShortcut = new QShortcut(Qt::Key_Y, this);
    m_yAlignShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_yAlignShortcut, SIGNAL(activated()), m_graphWidget, SLOT(yAlign()));

    m_xDistributeShortcut = new QShortcut(QKeySequence("Shift+X"), this);
    m_xDistributeShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_xDistributeShortcut, SIGNAL(activated()), m_graphWidget, SLOT(xDistribute()));

    m_yDistributeShortcut = new QShortcut(QKeySequence("Shift+Y"), this);
    m_yDistributeShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_yDistributeShortcut, SIGNAL(activated()), m_graphWidget, SLOT(yDistribute()));

    m_duplicateShortcut = new QShortcut(Qt::Key_D, this);
    m_duplicateShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_duplicateShortcut, SIGNAL(activated()), m_graphWidget, SLOT(duplicate()));

    m_groupShortcut = new QShortcut(Qt::Key_G, this);
    m_groupShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_groupShortcut, SIGNAL(activated()), m_graphWidget, SLOT(group()));

    m_ungroupShortcut = new QShortcut(Qt::Key_U, this);
    m_ungroupShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_ungroupShortcut, SIGNAL(activated()), m_graphWidget, SLOT(ungroup()));

    m_minimizeSelectedShortcut = new QShortcut(Qt::Key_M, this);
    m_minimizeSelectedShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_minimizeSelectedShortcut, SIGNAL(activated()), m_graphWidget, SLOT(minimizeSelected()));

    m_newTabShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this);
    m_newTabShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_newTabShortcut, SIGNAL(activated()), m_graphWidget, SLOT(newCuesheet()));

    // ----------------------------------------
    // GUI shortcuts
    m_reloadStylesShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    m_reloadStylesShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_reloadStylesShortcut, SIGNAL(activated()), guisettings, SLOT(loadStyleSheet()));

    // ----------------------------------------
    // File I/O shortcuts
#if 0
    m_saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    m_saveShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_saveShortcut, SIGNAL(activated()), this, SLOT(save()));

    m_saveAsShortcut = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);
    m_saveAsShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_saveAsShortcut, SIGNAL(activated()), this, SLOT(showSaveDialog()));

    m_openFileShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
    m_openFileShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_openFileShortcut, SIGNAL(activated()), this, SLOT(showOpenDialog()));

    m_importFileShortcut = new QShortcut(QKeySequence("Ctrl+I"), this);
    m_importFileShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_importFileShortcut, SIGNAL(activated()), this, SLOT(showImportDialog()));

    m_openSongShortcut = new QShortcut(QKeySequence("Ctrl+Shift+O"), this);
    m_openSongShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_openSongShortcut, SIGNAL(activated()), this, SLOT(showLoadSongDialog()));
#endif

    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(showLoadSongDialog()),
                    this, SLOT(showLoadSongDialog()));
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(showSaveDialog()),
                    this, SLOT(showSaveDialog()));
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(showOpenDialog()),
                    this, SLOT(showOpenDialog()));
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(showImportDialog()),
                    this, SLOT(showImportDialog()));
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(showSettingsDialog()),
                    this, SLOT(showSettingsDialog()));

    // Fowarding from Cupid to engine...
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(togglePlayback()),
                    m_engine, SLOT(togglePlayback()));
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(rewind()),
                    m_engine, SLOT(rewind()));
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(suspend()),
                    m_engine, SLOT(suspend()));
    CHECKED_CONNECT(Cupid::Singleton(), SIGNAL(startPlayback()),
                    m_engine, SLOT(startPlayback()));

    CHECKED_CONNECT(this, SIGNAL(updateButtonStates()),
                Cupid::Singleton(), SIGNAL(updateButtonStates()));
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
    emit updateButtonStates();

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

void MainWidget::audioPositionChanged(qint64 position)
{
    Q_UNUSED(position)
    // WTF why isn't this used?  Can we nuke it?
}

void MainWidget::bufferLengthChanged(qint64 length)
{
    m_transport->bufferLengthChanged(length);
}

//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

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
