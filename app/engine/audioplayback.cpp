//
//  Audio.cpp
//  Handles main timing and audio playback.
//  Calls the main dance floor update routine.
#include <math.h>

#include <QAudioOutput>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMetaObject>
#include <QSet>
#include <QThread>

#include "engine/audioplayback.h"
#include "engine/utils.h"
#include "engine/dancefloor.h"

Engine::Engine(QObject *parent)
    :   QObject(parent)
    ,   m_state(QAudio::StoppedState)
    ,   m_wavFileHandle(0)
    ,   m_audioOutputDevice(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_audioOutput(0)
    ,   m_playPosition(0)
    ,   m_spectrumBufferLength(0)
    ,   m_spectrumAnalyser()
    ,   m_notifyIntervalMs(35)
    ,   m_timer(new QTimer(this))
{
    qRegisterMetaType<FrequencySpectrum>("FrequencySpectrum");
    qRegisterMetaType<WindowFunction>("WindowFunction");
    CHECKED_CONNECT(&m_spectrumAnalyser,
                    SIGNAL(spectrumChanged(FrequencySpectrum)),
                    this,
                    SLOT(spectrumChanged(FrequencySpectrum)));

    // Set the main timer
    m_timer->setInterval(m_notifyIntervalMs);
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->start();
    CHECKED_CONNECT(m_timer, SIGNAL(timeout()), this, SLOT(updateDanceFloor()));
}

Engine::~Engine() { }

//-----------------------------------------------------------------------------
// Public functions

bool Engine::loadSong(const QString &fileName)
{
    Q_ASSERT(!fileName.isEmpty());
    m_audiofilename = fileName;
    reset();
    bool result = false;

    m_wavFileHandle = new WavFile(this);
    if (m_wavFileHandle->open(fileName)) {
        if (isPCMS16LE(m_wavFileHandle->fileFormat())) {
            result = initialize();
        } else {
            emit errorMessage(tr("Audio format not supported"), tr(""));
            return false;
        }
    } else {
        emit errorMessage(tr("Could not open file"), fileName);
        return false;
    }

    // Load whole file here.
    qint64 bytesToRead = m_wavFileHandle->size() - m_wavFileHandle->headerLength();
    m_buffer.resize(bytesToRead);
    int bytesRead = m_wavFileHandle->read(m_buffer.data(), bytesToRead);

    m_wavFileHandle->close();
    m_qbuf.setBuffer(&m_buffer);
    m_qbuf.open(QIODevice::ReadOnly);
    m_qbuf.seek(0);
    qDebug() << bytesToRead << bytesRead << m_qbuf.size() << m_audioOutput->bufferSize();
    emit bufferLengthChanged(bufferLengthBytes());
    emit newSong(fileName);

    return result;
}

qint64 Engine::bufferLengthBytes() const
{
    // Return buffer length in bytes
    return m_qbuf.buffer().size();
}

qint64 Engine::bufferLengthMS() const
{
    // Return buffer length in ms
    return audioDuration(m_format, bufferLengthBytes());
}

void Engine::setWindowFunction(WindowFunction type)
{
    m_spectrumAnalyser.setWindowFunction(type);
}

//-----------------------------------------------------------------------------
// Public slots

void Engine::startPlayback()
{
    if (QAudio::SuspendedState == m_state) {
        m_audioOutput->resume();
    } else {
        m_spectrumAnalyser.cancelCalculation();
        spectrumChanged(0, 0, FrequencySpectrum());
        setPlayPosition(0, true);
        CHECKED_CONNECT(m_audioOutput, SIGNAL(stateChanged(QAudio::State)),
                        this, SLOT(audioStateChanged(QAudio::State)));
        m_qbuf.seek(0);
        m_audioOutput->start(&m_qbuf);
    }
}

void Engine::rewind()
{
    movePlaybackHead(0.0);
}

void Engine::movePlaybackHead(double positionfraction)
{
    // Sets playback position to somewhere in middle of recording.
    // position == 0.0 means the beginning,
    // position == 1.0 means the end.

    qint64 byteposition = positionfraction * bufferLengthBytes();

    // Round off to start of the channel 0.
    byteposition -= byteposition % (m_format.channelCount() * m_format.sampleSize());
    emit playPositionChanged(byteposition);

    bool result = m_qbuf.seek(byteposition);
    Q_ASSERT(result);
}

void Engine::suspend()
{
    if (QAudio::ActiveState == m_state || QAudio::IdleState == m_state) {
            m_audioOutput->suspend();
    }
}

void Engine::togglePlayback()
{
    if (QAudio::ActiveState == m_state)
        m_audioOutput->suspend();
    else if (QAudio::IdleState == m_state)
        m_audioOutput->resume();
    else
        startPlayback();
}

//-----------------------------------------------------------------------------
// Private slots

void Engine::updateDanceFloor()
{
    // Callback
    // The tail that wags the dog!
    // Updates the spectrum, then calls the dance floor to update all
    // the pixels.
    if (QAudio::ActiveState == m_state) {
        const qint64 playPosition = m_qbuf.pos();
        const qint64 spectrumPosition = playPosition - m_spectrumBufferLength;

        const qint64 specWindowStart = qMax(qint64(0), spectrumPosition);
        const qint64 specWindowEnd   = qMin(m_qbuf.size(), spectrumPosition + m_spectrumBufferLength);

        calculateSpectrum(specWindowStart, specWindowEnd);

        // ==============================
        // OKAY!
        // THIS IS IT!  ARE YOU READY?  It all happens here:

        m_dfModel->evaluate();

        // TA-DA !
        // ==============================
    }
}

void Engine::audioStateChanged(QAudio::State state)
{
    qDebug() << "Engine::audioStateChanged from " << m_state << " to " << state;

    // Check error
    QAudio::Error error = QAudio::NoError;
    error = m_audioOutput->error();
    if (error != QAudio::NoError) {
        qDebug() << "Engine::audioStateChanged [0] error " << error ;
        qDebug() << "_qbuf.pos()" << m_qbuf.pos();
    }

    if (state == QAudio::IdleState && m_qbuf.pos() == m_qbuf.size()) {
        // The song is over!
        stopPlayback();
    } else {
        if (state == QAudio::StoppedState) {
            // Check error
            QAudio::Error error = QAudio::NoError;
            error = m_audioOutput->error();
            if (QAudio::NoError != error) {
                qDebug() << "Engine::audioStateChanged error " << error ;
                reset();
                return;
            }
        }
        setState(state);
    }
}

void Engine::spectrumChanged(const FrequencySpectrum &spectrum)
{
    emit spectrumChanged(0, m_spectrumBufferLength, spectrum);
}


//-----------------------------------------------------------------------------
// Private functions

void Engine::resetAudioDevices()
{
    delete m_audioOutput;
    m_audioOutput = 0;
    setPlayPosition(0);
}

void Engine::reset()
{
    stopPlayback();
    setState(QAudio::StoppedState);
    setFormat(QAudioFormat());
    delete m_wavFileHandle;
    m_wavFileHandle = 0;
    m_buffer.clear();
    if (m_qbuf.isOpen())
        m_qbuf.close();
    resetAudioDevices();
}

bool Engine::initialize()
{
    bool result = false;

    if (selectFormat()) {
        resetAudioDevices();
        result = true;
        m_audioOutput = new QAudioOutput(m_format, this);
    } else {
        if (m_wavFileHandle)
            emit errorMessage(tr("Audio format not supported"), tr(""));
    }

    return result;
}

bool Engine::selectFormat()
{
    bool foundSupportedFormat = false;

    if (m_wavFileHandle) {
        QAudioFormat format = m_format;
        // Header is read from the WAV file; just need to check whether
        // it is supported by the audio output device
        format = m_wavFileHandle->fileFormat();
        if (m_audioOutputDevice.isFormatSupported(format)) {
            setFormat(format);
            foundSupportedFormat = true;
        }
    }

    return foundSupportedFormat;
}

void Engine::stopPlayback()
{
    if (m_audioOutput) {
        m_audioOutput->stop();
        QCoreApplication::instance()->processEvents();
        m_audioOutput->disconnect();
        setPlayPosition(0);
    }
}

void Engine::setState(QAudio::State state)
{
    const bool changed = (m_state != state);
    if (changed) {
        ENGINE_DEBUG << "Engine::setState from " << m_state
                     << " to " << state;
        m_state = state;
        emit stateChanged(m_state);
    }
}

// ??? should we nuke this, and get play position 'live'?
// or does the check-for-pos-changed save updates?
void Engine::setPlayPosition(qint64 position, bool forceEmit)
{
    const bool changed = (m_playPosition != position);
    m_playPosition = position;
    if (changed || forceEmit)
        emit playPositionChanged(m_playPosition);
}

void Engine::calculateSpectrum(qint64 start, qint64 end)
{
    // QThread::currentThread is marked 'for internal use only', but
    // we're only using it for debug output here, so it's probably OK :)
    ENGINE_DEBUG << "Engine::calculateSpectrum" << QThread::currentThread()
                 << "spectrumAnalyser.isReady" << m_spectrumAnalyser.isReady();

    if (m_spectrumAnalyser.isReady()) {
        // ??? are we sizing & clearing correctly???
        // Must copy data into spectrum buffer because Hahn window will be applied.
        // Note fromRawData uses implicit shared array.
        m_spectrumBuffer = QByteArray::fromRawData(m_buffer.constData() + start,
                                                   m_spectrumBufferLength);
        qint64 len = end-start+1;
        if (len < m_spectrumBufferLength) {
            // TODO make sure this reads correct shortened buffer
            ENGINE_DEBUG << Q_FUNC_INFO << "len" << len << "m_specbuflen" << m_spectrumBufferLength;
        }
        m_spectrumAnalyser.calculate(m_spectrumBuffer, m_format);
    }

}

void Engine::setFormat(const QAudioFormat &format)
{
    const bool changed = (format != m_format);
    m_format = format;
    m_spectrumBufferLength = SPECTRUM_LENGTH_SAMPLES *
                            (m_format.sampleSize() / 8) * m_format.channelCount();
    if (changed)
        emit formatChanged(m_format);
}

/*
 * Sets interval at which spectrum is updated.
 * NOTE:  this is also the interval at which the whole floor is
 * updated, and the two should probably be decoupled.
 */
void Engine::setUpdateInterval(int val)
{
    m_notifyIntervalMs = val;
    m_audioOutput->setNotifyInterval(m_notifyIntervalMs);
}

// return time in microseconds
qint64 Engine::getCurrentTime() const
{
    qint64 positionInBytes = m_qbuf.pos();
    return audioDuration(m_format, positionInBytes);
}
