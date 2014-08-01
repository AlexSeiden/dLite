#include "engine.h"
#include "levelmeter.h"
#include "sublevel.h"
#include "mainwidget.h"
#include "progressbar.h"
#include "settingsdialog.h"
#include "spectrograph.h"
#include "utils.h"
#include "dancefloorcontroller.h"

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
    ,   m_levelMeter(new LevelMeter(this))
    ,   m_subrangeLevelMeter(new SublevelMeter(this))
    ,   m_fileButton(new QPushButton(this))
    ,   m_pauseButton(new QPushButton(this))
    ,   m_playButton(new QPushButton(this))
    ,   m_settingsButton(new QPushButton(this))
    ,   m_printSpectrum(new QPushButton(this))
    ,   m_numBandsSpinBox(new QSpinBox(this))
    ,   m_specMinSpinBox(new QSpinBox(this))
    ,   m_specMaxSpinBox(new QSpinBox(this))
    ,   m_infoMessage(new QLabel(tr("Select a file to begin"), this))
    ,   m_infoMessageTimerId(NullTimerId)
    ,   m_settingsDialog(new SettingsDialog(m_engine->interval(), this))
    ,   m_loadFileAction(0)
    ,   m_DancefloorController(new DancefloorController)
{
    // numBands, lowfreq, hifreq.
    // TODO move somewhere else....
    m_spectrograph->setParams(20, 20., 10000.);

    createUi();
    connectUi();

    // TODO default to last played.
    m_engine->loadFile(QString("/Users/alex/Documents/lights/Jam On It/Jam On It.wav"));
    m_DancefloorController->setup("/Users/alex/src/floorit/layout.csv");
}

MainWidget::~MainWidget()
{

}


//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void MainWidget::stateChanged(QAudio::State state)
{
    updateButtonStates();

    if (QAudio::ActiveState != state && QAudio::SuspendedState != state) {
        m_levelMeter->reset();
        m_spectrograph->reset();
    }
}

void MainWidget::formatChanged(const QAudioFormat &format)
{
   infoMessage(formatToString(format), NullMessageTimeout);
}

void MainWidget::spectrumChanged(qint64 position, qint64 length,
                                 const FrequencySpectrum &spectrum)
{
    m_progressBar->windowChanged(position, length);
    m_spectrograph->spectrumChanged(spectrum);
}

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
    const QString dir;
    const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open WAV file"), dir, "*.wav");
    if (fileNames.count()) {
        reset();
        m_engine->loadFile(fileNames.front());
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
    windowLayout->addWidget(m_infoMessage);

    windowLayout->addWidget(m_progressBar);

    // Spectrograph and level meter
    QScopedPointer<QHBoxLayout> analysisLayout(new QHBoxLayout);
    analysisLayout->addWidget(m_levelMeter);
    analysisLayout->addWidget(m_spectrograph);
    analysisLayout->addWidget(m_subrangeLevelMeter);
    windowLayout->addLayout(analysisLayout.data());
    analysisLayout.take();

    // Button panel
    const QSize buttonSize(30, 30);

    m_fileButton->setText(tr("File..."));
    m_printSpectrum->setText(tr("print spectrum"));

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

    QScopedPointer<QHBoxLayout> buttonPanelLayout(new QHBoxLayout);
    buttonPanelLayout->addStretch();
    buttonPanelLayout->addWidget(m_fileButton);
    buttonPanelLayout->addWidget(m_pauseButton);
    buttonPanelLayout->addWidget(m_playButton);
    buttonPanelLayout->addWidget(m_settingsButton);
    buttonPanelLayout->addWidget(m_printSpectrum);

    QWidget *buttonPanel = new QWidget(this);
    buttonPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    buttonPanel->setLayout(buttonPanelLayout.data());
    buttonPanelLayout.take(); // ownership transferred to buttonPanel

    QScopedPointer<QHBoxLayout> bottomPaneLayout(new QHBoxLayout);
    bottomPaneLayout->addWidget(buttonPanel);
    windowLayout->addLayout(bottomPaneLayout.data());
    bottomPaneLayout.take(); // ownership transferred to windowLayout


    // Options layout
    QLabel *numBandsLabel = new QLabel(tr("Number of Bands"), this);
    m_numBandsSpinBox->setMinimum(3);
    m_numBandsSpinBox->setMaximum(40);
    m_numBandsSpinBox->setFixedWidth(50);
    m_numBandsSpinBox->setValue(m_spectrograph->numBars());
    QScopedPointer<QHBoxLayout> numBandsLayout (new QHBoxLayout);
    numBandsLayout->addWidget(numBandsLabel);
    numBandsLayout->addWidget(m_numBandsSpinBox);
    numBandsLayout->addStretch();

    QLabel *specMinLabel = new QLabel(tr("Min Freq"), this);
    m_specMinSpinBox->setMinimum(0);
    m_specMinSpinBox->setMaximum(20000);
    m_specMinSpinBox->setFixedWidth(70);
    m_specMinSpinBox->setValue(m_spectrograph->freqLo());
    QScopedPointer<QHBoxLayout> specMinLayout (new QHBoxLayout);
    specMinLayout->addWidget(specMinLabel);
    specMinLayout->addWidget(m_specMinSpinBox);
    specMinLayout->addStretch();

    QLabel *specMaxLabel = new QLabel(tr("Max Freq"), this);
    m_specMaxSpinBox->setMinimum(0);
    m_specMaxSpinBox->setMaximum(40000);
    m_specMaxSpinBox->setFixedWidth(70);
    m_specMaxSpinBox->setValue(m_spectrograph->freqHi());
    QScopedPointer<QHBoxLayout> specMaxLayout (new QHBoxLayout);
    specMaxLayout->addWidget(specMaxLabel);
    specMaxLayout->addWidget(m_specMaxSpinBox);
    specMaxLayout->addStretch();

    QScopedPointer<QHBoxLayout> optionsLayout(new QHBoxLayout);
    optionsLayout->addLayout(numBandsLayout.data());
    optionsLayout->addLayout(specMinLayout.data());
    optionsLayout->addLayout(specMaxLayout.data());
    numBandsLayout.take(); 			// ownership transferred to options layout
    specMinLayout.take(); 			// ownership transferred to options layout
    specMaxLayout.take(); 			// ownership transferred to options layout
    //optionsLayout->addStretch();	// Fills empty space rather than stretching out sub-layouts.

    windowLayout->addLayout(optionsLayout.data());
    optionsLayout.take(); // ownership transferred to windowLayout

    // Apply layout
    setLayout(windowLayout);
}

void MainWidget::connectUi()
{
    CHECKED_CONNECT(m_printSpectrum, SIGNAL(clicked()),
            m_spectrograph, SLOT(printSpectrum()));

    CHECKED_CONNECT(m_pauseButton, SIGNAL(clicked()),
            m_engine, SLOT(suspend()));

    CHECKED_CONNECT(m_playButton, SIGNAL(clicked()),
            m_engine, SLOT(startPlayback()));

    CHECKED_CONNECT(m_fileButton, SIGNAL(clicked()),
            this, SLOT(showFileDialog()));

    CHECKED_CONNECT(m_settingsButton, SIGNAL(clicked()),
            this, SLOT(showSettingsDialog()));

    CHECKED_CONNECT(m_engine, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(stateChanged(QAudio::State)));

    CHECKED_CONNECT(m_engine, SIGNAL(formatChanged(const QAudioFormat &)),
            this, SLOT(formatChanged(const QAudioFormat &)));

    m_progressBar->bufferLengthChanged(m_engine->bufferLength());

    CHECKED_CONNECT(m_engine, SIGNAL(bufferLengthChanged(qint64)),
            this, SLOT(bufferLengthChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(dataLengthChanged(qint64)),
            this, SLOT(updateButtonStates()));

    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
            m_progressBar, SLOT(playPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
            this, SLOT(audioPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(levelChanged(qreal, qreal, int)),
            m_levelMeter, SLOT(levelChanged(qreal, qreal, int)));

    CHECKED_CONNECT(m_spectrograph, SIGNAL(subrangeLevelChanged(qreal, qreal, int)),
            m_subrangeLevelMeter, SLOT(levelChanged(qreal, qreal, int)));

    CHECKED_CONNECT(m_engine, SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
            this, SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));

    CHECKED_CONNECT(m_engine, SIGNAL(infoMessage(QString, int)),
            this, SLOT(infoMessage(QString, int)));

    CHECKED_CONNECT(m_engine, SIGNAL(errorMessage(QString, QString)),
            this, SLOT(errorMessage(QString, QString)));

    CHECKED_CONNECT(m_spectrograph, SIGNAL(infoMessage(QString, int)),
            this, SLOT(infoMessage(QString, int)));

    CHECKED_CONNECT(m_numBandsSpinBox, SIGNAL(valueChanged(int)),
            m_spectrograph, SLOT(setNumBars(int)));

    // TODO these should only signal when enter is hit or something.
    // And they probably shouldn't be spinboxes; or at least should increment
    // geometrically.
    CHECKED_CONNECT(m_specMinSpinBox, SIGNAL(valueChanged(int)),
            m_spectrograph, SLOT(setFreqLo(int)));

    CHECKED_CONNECT(m_specMaxSpinBox, SIGNAL(valueChanged(int)),
            m_spectrograph, SLOT(setFreqHi(int)));
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
    m_levelMeter->reset();
    m_spectrograph->reset();
    m_progressBar->reset();
}
