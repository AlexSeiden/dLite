#include "engine.h"
#include "mainwidget.h"
#include "Transport.h"
#include "settingsdialog.h"
#include "spectrograph.h"
#include "utils.h"
#include "dancefloorwidget.h"
#include "CueLibView.h"
#include "GraphWidget.h"
#include "Node.h"
#include "Cupid.h"
#include "NodeFactory.h"

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

const int NullTimerId = -1;

MainWidget::MainWidget(QWidget *parent)
    :   QWidget(parent)
    ,   m_engine(new Engine(this))
    ,   m_transport(new Transport(this))
    ,   m_spectrograph(new Spectrograph(this))
    ,   m_fileButton(new QPushButton(this))
    ,   m_saveButton(new QPushButton(this))
    ,   m_openButton(new QPushButton(this))
    ,   m_pauseButton(new QPushButton(this))
    ,   m_playButton(new QPushButton(this))
    ,   m_settingsButton(new QPushButton(this))
    ,   m_infoMessage(new QLabel(tr(""), this))
    ,   m_infoMessageTimerId(NullTimerId)
    ,   m_settingsDialog(new SettingsDialog(m_engine->interval(), this))
    ,   m_dancefloor(new Dancefloor)
    ,   m_cueLibView(NULL)
    ,   _filename()
{
    // GROSS  These are essentially globals:
    Cupid::Singleton()->setEngine(m_engine);
    Cupid::Singleton()->setDancefloor(m_dancefloor);
    Cupid::Singleton()->setSpectrograph(m_spectrograph);
    Cupid::Singleton()->setTransport(m_transport);

    // numBands, lowfreq, hifreq.
    // TODO move somewhere else....
    m_spectrograph->setParams(20, 20., 10000.);

    createUi();

    setMinimumSize(500, 400);
    resize(600, 450);
    move(5,10);  // TODO restore from last saved

    // TODO move to settings/prefs  & allow setting this
    std::string layoutfile = std::string("/Users/alex/src/floorit/layout.csv");
    m_dancefloor->ImportLayout(layoutfile);
    Cue::setDancefloor(m_dancefloor);
    m_engine->setDancefloormodel(m_dancefloor);

    m_dancefloorwidget = new Dancefloorwidget();
    m_dancefloorwidget->setModel(m_dancefloor);
    m_dancefloor->setView(m_dancefloorwidget);  // GROSS
    m_dancefloorwidget->show();
    Cupid::Singleton()->setDancefloorwidget(m_dancefloorwidget);


    m_cueLibView = new CueLibView(NULL);
    m_cueLibView->show();

    m_graphWidget = new GraphWidget(NULL);
    m_graphWidget->show();
    Cupid::Singleton()->setGraphWidget(m_graphWidget);

    connectUi();

    // TODO default to last played.
    //m_engine->loadFile(QString("/Users/alex/Documents/lights/Jam On It/Jam On It.wav"));
    m_engine->loadSong(QString("/Users/alex/Documents/WAVS/Awesome/Awesome.wav"));
    updateButtonStates();

    createShortcuts();
    createActions();
    createMenus();
}

MainWidget::~MainWidget() { }


//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void MainWidget::stateChanged(QAudio::State state)
{
    updateButtonStates();

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
    if (_filename.isEmpty() || _filename.isNull())
        showSaveDialog();
    else
        NodeFactory::Singleton()->saveToFile(_filename);
}

#if 0 // NUKEMEMAYBE
void MainWidget::infoMessage(const QString &message, int timeoutMs)
{
    m_infoMessage->setText(message);

    if (NullTimerId != m_infoMessageTimerId) {
        killTimer(m_infoMessageTimerId);
        m_infoMessageTimerId = NullTimerId;
    }

    if (NullMessageTimeout != timeoutMs)
        m_infoMessageTimerId = startTimer(timeoutMs);
}

void MainWidget::errorMessage(const QString &heading, const QString &detail)
{
    QMessageBox::warning(this, heading, detail, QMessageBox::Close);
}
#endif


void MainWidget::timerEvent(QTimerEvent *event)
{
    Q_ASSERT(event->timerId() == m_infoMessageTimerId);
    Q_UNUSED(event) // suppress warnings in release builds
    killTimer(m_infoMessageTimerId);
    m_infoMessageTimerId = NullTimerId;
    m_infoMessage->setText("");
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

void MainWidget::showLoadSongDialog()
{
    // TODO Load & queue multiple files
    QString dir = "/Users/alex/Documents/WAVS";
    const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open WAV file"), dir, "*.wav");
    if (fileNames.count()) {
        reset();
        m_engine->loadSong(fileNames.front()); // TODO open multiple files
        updateButtonStates();
        m_engine->startPlayback();
    }
}

void MainWidget::showSaveDialog()
{
    const QString dir = QDir::homePath(); // XXX better default path
    _filename = QFileDialog::getSaveFileName(this, tr("Save file"), dir);
    if (! _filename.isEmpty()) {
        if (! _filename.endsWith(".dlite"))
            _filename.append(".dlite");
        NodeFactory::Singleton()->saveToFile(_filename);
    }
}

void MainWidget::showOpenDialog()
{
    const QString dir = QDir::homePath(); // XXX better default path
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open dLite file"), dir, "*.dLite");
    // TODO open multiple files
    if (! fileName.isEmpty() && ! fileName.isNull()) {
        bool result = NodeFactory::Singleton()->readFromFile(fileName);
        if (result)
            _filename = fileName;
        updateButtonStates();
    }
}

void MainWidget::showImportDialog()
{
    const QString dir = QDir::homePath(); // XXX better default path
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Import dLite file"), dir, "*.dLite");
    // TODO open multiple files
    if (! fileName.isEmpty() && ! fileName.isNull()) {
        NodeFactory::Singleton()->readFromFile(fileName, true);
        updateButtonStates();
    }
}

void MainWidget::showSettingsDialog()
{
    m_settingsDialog->exec();
    if (m_settingsDialog->result() == QDialog::Accepted) {
        m_engine->setWindowFunction(m_settingsDialog->windowFunction());
        m_engine->setUpdateInterval(m_settingsDialog->interval());
        m_dancefloor->setHardwareStatus(m_settingsDialog->useHardware());
    }
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MainWidget::createUi()
{
    setWindowTitle(tr("Spectrum Analyser"));

    QVBoxLayout *windowLayout = new QVBoxLayout(this);

    m_infoMessage->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_infoMessage->setAlignment(Qt::AlignHCenter);

    windowLayout->addWidget(m_transport);

    // Spectrograph
    QScopedPointer<QHBoxLayout> analysisLayout(new QHBoxLayout);
    analysisLayout->addWidget(m_spectrograph);
    windowLayout->addLayout(analysisLayout.data());
    analysisLayout.take();

    // Button panel
    const QSize buttonSize(30, 30);

    m_fileButton->setText(tr("Song..."));

    m_pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
    m_pauseButton->setIcon(m_pauseIcon);
    m_pauseButton->setEnabled(false);
    m_pauseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pauseButton->setMinimumSize(buttonSize);

    m_playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    m_playButton->setIcon(m_playIcon);
    m_playButton->setEnabled(false);
    m_playButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_playButton->setMinimumSize(buttonSize);

    m_settingsIcon = QIcon(":/images/settings.png");
    m_settingsButton->setIcon(m_settingsIcon);
    m_settingsButton->setEnabled(true);
    m_settingsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_settingsButton->setMinimumSize(buttonSize);

    m_saveButton->setText(tr("Save..."));
    m_openButton->setText(tr("Open..."));


    QScopedPointer<QHBoxLayout> buttonPanelLayout(new QHBoxLayout);
    buttonPanelLayout->addStretch();
    buttonPanelLayout->addWidget(m_fileButton);
    buttonPanelLayout->addWidget(m_pauseButton);
    buttonPanelLayout->addWidget(m_playButton);
    buttonPanelLayout->addWidget(m_settingsButton);
    buttonPanelLayout->addWidget(m_openButton);
    buttonPanelLayout->addWidget(m_saveButton);

    QWidget *buttonPanel = new QWidget(this);
    buttonPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    buttonPanel->setLayout(buttonPanelLayout.data());
    buttonPanelLayout.take(); // ownership transferred to buttonPanel

    QScopedPointer<QHBoxLayout> bottomPaneLayout(new QHBoxLayout);
    bottomPaneLayout->addWidget(buttonPanel);
    windowLayout->addLayout(bottomPaneLayout.data());
    bottomPaneLayout.take(); // ownership transferred to windowLayout

    // Apply layout
    setLayout(windowLayout);
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
    CHECKED_CONNECT(m_pauseButton, SIGNAL(clicked()),
            m_engine, SLOT(suspend()));

    CHECKED_CONNECT(m_playButton, SIGNAL(clicked()),
            m_engine, SLOT(startPlayback()));

    CHECKED_CONNECT(m_fileButton, SIGNAL(clicked()),
            this, SLOT(showLoadSongDialog()));

    CHECKED_CONNECT(m_saveButton, SIGNAL(clicked()),
            this, SLOT(showSaveDialog()));

    CHECKED_CONNECT(m_openButton, SIGNAL(clicked()),
            this, SLOT(showOpenDialog()));

    CHECKED_CONNECT(m_settingsButton, SIGNAL(clicked()),
            this, SLOT(showSettingsDialog()));

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

#if 0 // NUKEMEMAYBE
    // Info & error messages--now obsolete?
    CHECKED_CONNECT(m_engine, SIGNAL(infoMessage(QString, int)),
            this, SLOT(infoMessage(QString, int)));

    CHECKED_CONNECT(m_engine, SIGNAL(errorMessage(QString, QString)),
            this, SLOT(errorMessage(QString, QString)));

    CHECKED_CONNECT(m_spectrograph, SIGNAL(infoMessage(QString, int)),
            this, SLOT(infoMessage(QString, int)));
#endif


    CHECKED_CONNECT(m_cueLibView, SIGNAL(newNodeRequest(QString)),
                    this, SLOT(newNodeRequest(QString)));
}

void MainWidget::createShortcuts()
{
    // ----------------------------------------
    // Transport shortcuts
    m_playPauseShortcut = new QShortcut(Qt::Key_Space, this);
    m_playPauseShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_playPauseShortcut, SIGNAL(activated()), m_engine, SLOT(togglePlayback()));

    m_rewindShortcut = new QShortcut(QKeySequence("Ctrl+0"), this);
    m_rewindShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_rewindShortcut, SIGNAL(activated()), m_engine, SLOT(rewind()));

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
    // File I/O shortcuts
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
}

void MainWidget::updateButtonStates()
{
    const bool pauseEnabled = (QAudio::ActiveState == m_engine->state() ||
                               QAudio::IdleState == m_engine->state());
    m_pauseButton->setEnabled(pauseEnabled);

    const bool playEnabled = ( (QAudio::ActiveState != m_engine->state() &&
                                QAudio::IdleState != m_engine->state()));
    m_playButton->setEnabled(playEnabled);
}

void MainWidget::reset()
{
    m_engine->reset();
    m_spectrograph->reset();
    m_transport->reset();
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
