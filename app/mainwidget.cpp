#include "engine.h"
#include "sublevel.h"
#include "mainwidget.h"
#include "progressbar.h"
#include "settingsdialog.h"
#include "spectrograph.h"
#include "utils.h"
#include "dancefloorwidget.h"
#include "CueLibView.h"
#include "GraphWidget.h"
#include "Node.h"
#include "Cupid.h"

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

const int NullTimerId = -1;

MainWidget::MainWidget(QWidget *parent)
    :   QWidget(parent)
    ,   m_engine(new Engine(this))
    ,   m_progressBar(new ProgressBar(this))
    ,   m_spectrograph(new Spectrograph(this))
    ,   m_fileButton(new QPushButton(this))
    ,   m_saveButton(new QPushButton(this))
    ,   m_openButton(new QPushButton(this))
    ,   m_pauseButton(new QPushButton(this))
    ,   m_playButton(new QPushButton(this))
    ,   m_settingsButton(new QPushButton(this))
//    ,   m_numBandsSpinBox(new QSpinBox(this))
//    ,   m_specMinSpinBox(new QSpinBox(this))
//    ,   m_specMaxSpinBox(new QSpinBox(this))
    ,   m_infoMessage(new QLabel(tr(""), this))
    ,   m_infoMessageTimerId(NullTimerId)
    ,   m_settingsDialog(new SettingsDialog(m_engine->interval(), this))
    ,   m_loadFileAction(0)
    ,   m_dancefloormodel(new Dancefloor)  // TODO should be allocated in main?
    ,   m_cueLibView(NULL)
{

    // numBands, lowfreq, hifreq.
    // TODO move somewhere else....
    m_spectrograph->setParams(20, 20., 10000.);

    createUi();

    setMinimumSize(500, 400);
    move(5,10);  // TODO restore from last saved

    // TODO move to settings/prefs  & allow setting this
    std::string layoutfile = std::string("/Users/alex/src/floorit/layout.csv");
    m_dancefloormodel->ImportLayout(layoutfile);
    Cue::setDancefloor(m_dancefloormodel);

    m_dancefloorwidget = new Dancefloorwidget();
    m_dancefloorwidget->setModel(m_dancefloormodel);
    m_dancefloormodel->setView(m_dancefloorwidget);  // GROSS
    m_dancefloorwidget->show();

    m_engine->setDancefloormodel(m_dancefloormodel);

    m_cueLibView = new CueLibView(NULL);
    m_cueLibView->show();

    m_graphWidget = new GraphWidget(NULL);
    m_graphWidget->show();

    connectUi();

    // TODO default to last played.
    //m_engine->loadFile(QString("/Users/alex/Documents/lights/Jam On It/Jam On It.wav"));
    m_engine->loadFile(QString("/Users/alex/Documents/WAVS/Awesome/Awesome.wav"));


    // OK, these are essentially globals:  GROSS
    Cupid::Singleton()->setSpectrograph(m_spectrograph);
    Cupid::Singleton()->setDancefloor(m_dancefloormodel);
    Cupid::Singleton()->setEngine(m_engine);
    Cupid::Singleton()->setGraphWidget(m_graphWidget);
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
    // ??? Why are these implemented as function calls, instead of signals?
    m_progressBar->windowChanged(position, length);
    m_spectrograph->spectrumChanged(spectrum);
}

#ifdef NUKEME
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
    m_progressBar->bufferLengthChanged(length);
}


//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void MainWidget::showFileDialog()
{
    // TODO Load & queue multiple files
    const QString dir;
    const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open WAV file"), dir, "*.wav");
    if (fileNames.count()) {
        reset();
        m_engine->loadFile(fileNames.front()); // TODO open multiple files
        updateButtonStates();
    }
}

void MainWidget::showSaveDialog()
{
    const QString dir = QDir::homePath(); // XXX
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), dir);
    if (! fileName.isEmpty()) {
        NodeFactory::Singleton()->saveToFile(fileName);
    }
}

void MainWidget::showOpenDialog()
{
    const QString dir = QDir::homePath(); // XXX
    const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open dLite file"), dir, "*.json *.dLite");
    if (fileNames.count()) {
        QString fileName = fileNames.front(); // TODO open multiple files
        NodeFactory::Singleton()->readFromFile(fileName);
        m_graphWidget->addAllNodes(NodeFactory::Singleton()->allNodes());
        updateButtonStates();
    }
}

void MainWidget::showSettingsDialog()
{
    m_settingsDialog->exec();
    if (m_settingsDialog->result() == QDialog::Accepted) {
        m_engine->setWindowFunction(m_settingsDialog->windowFunction());
        m_engine->setInterval(m_settingsDialog->interval());
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

    windowLayout->addWidget(m_progressBar);

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

void MainWidget::connectUi()
{
    CHECKED_CONNECT(m_pauseButton, SIGNAL(clicked()),
            m_engine, SLOT(suspend()));

    CHECKED_CONNECT(m_playButton, SIGNAL(clicked()),
            m_engine, SLOT(startPlayback()));

    CHECKED_CONNECT(m_fileButton, SIGNAL(clicked()),
            this, SLOT(showFileDialog()));

    CHECKED_CONNECT(m_saveButton, SIGNAL(clicked()),
            this, SLOT(showSaveDialog()));

    CHECKED_CONNECT(m_openButton, SIGNAL(clicked()),
            this, SLOT(showOpenDialog()));

    CHECKED_CONNECT(m_settingsButton, SIGNAL(clicked()),
            this, SLOT(showSettingsDialog()));

    CHECKED_CONNECT(m_engine, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(stateChanged(QAudio::State)));

    m_progressBar->bufferLengthChanged(m_engine->bufferLength());

    CHECKED_CONNECT(m_engine, SIGNAL(bufferLengthChanged(qint64)),
            this, SLOT(bufferLengthChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(dataLengthChanged(qint64)),
            this, SLOT(updateButtonStates()));

    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
            m_progressBar, SLOT(playPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
            this, SLOT(audioPositionChanged(qint64)));

    CHECKED_CONNECT(m_spectrograph, SIGNAL(subrangeHasChanged(Subrange *)),
                    m_graphWidget, SLOT(subrangeHasChanged(Subrange*)));

    CHECKED_CONNECT(m_spectrograph, SIGNAL(subrangeHasChanged(Subrange*)),
            m_graphWidget, SLOT(subrangeHasChanged(Subrange*)));

    CHECKED_CONNECT(m_engine, SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
            this, SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));


#ifdef NUKEME
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

#if 0
// TODO find a home for this, either in settings or spectrograph
QLayout *createSpectrumOptionsUI(QWidget *parent, Spectrograph *spectrograph)
{

    QHBoxLayout *layout = new QHBoxLayout;

    // Options layout
    QLabel *numBandsLabel = new QLabel(tr("Number of Bands"), parent);
    QSpinBox *numBandsSpinBox = new QSpinBox(parent);
    numBandsSpinBox->setMinimum(3);
    numBandsSpinBox->setMaximum(40);
    numBandsSpinBox->setFixedWidth(50);
    numBandsSpinBox->setValue(spectrograph->numBars());
    QScopedPointer<QHBoxLayout> numBandsLayout (new QHBoxLayout);
    numBandsLayout->addWidget(numBandsLabel);
    numBandsLayout->addWidget(numBandsSpinBox);
    numBandsLayout->addStretch();
    layout->addLayout(numBandsLayout.data());
    numBandsLayout.take();

    QLabel *specMinLabel = new QLabel(tr("Min Freq"), parent);
    QSpinBox *specMinSpinBox = new QSpinBox(parent);
    specMinSpinBox->setMinimum(0);
    specMinSpinBox->setMaximum(20000);
    specMinSpinBox->setFixedWidth(70);
    specMinSpinBox->setValue(spectrograph->freqLo());
    QScopedPointer<QHBoxLayout> specMinLayout (new QHBoxLayout);
    specMinLayout->addWidget(specMinLabel);
    specMinLayout->addWidget(specMinSpinBox);
    specMinLayout->addStretch();
    layout->addLayout(specMinLayout.data());
    specMinLayout.take();

    QLabel *specMaxLabel = new QLabel(tr("Max Freq"), parent);
    QSpinBox *specMaxSpinBox = new QSpinBox(parent);
    specMaxSpinBox->setMinimum(0);
    specMaxSpinBox->setMaximum(40000);
    specMaxSpinBox->setFixedWidth(70);
    specMaxSpinBox->setValue(spectrograph->freqHi());
    QScopedPointer<QHBoxLayout> specMaxLayout (new QHBoxLayout);
    specMaxLayout->addWidget(specMaxLabel);
    specMaxLayout->addWidget(specMaxSpinBox);
    specMaxLayout->addStretch();
    layout->addLayout(specMaxLayout.data());
    specMaxLayout.take(); // ownership transferred to dialogLayout

    // TODO Move to Spectrograph, or settings dialog
    CHECKED_CONNECT(numBandsSpinBox, SIGNAL(valueChanged(int)),
            spectrograph, SLOT(setNumBars(int)));

    // TODO these probably shouldn't be spinboxes; or at least should increment
    // geometrically.
    CHECKED_CONNECT(specMinSpinBox, SIGNAL(valueChanged(int)),
            spectrograph, SLOT(setFreqLo(int)));

    CHECKED_CONNECT(specMaxSpinBox, SIGNAL(valueChanged(int)),
            spectrograph, SLOT(setFreqHi(int)));

    return layout;
}
#endif

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
    m_progressBar->reset();
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
