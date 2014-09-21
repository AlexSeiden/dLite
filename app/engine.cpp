#include "engine.h"
#include "utils.h"
#include "DanceFloor.h"

#include <math.h>

#include <QAudioOutput>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMetaObject>
#include <QSet>
#include <QThread>

Engine::Engine(QObject *parent)
    :   QObject(parent)
    ,   m_state(QAudio::StoppedState)
    ,   m_wavFileHandle(0)
    ,   m_analysisFile(0)
    ,   m_audioOutputDevice(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_audioOutput(0)
    ,   m_playPosition(0)
    ,   m_bufferPosition(0)
    ,   m_spectrumBufferLength(0)
    ,   m_spectrumAnalyser()
    ,   m_spectrumPosition(0)
    ,   m_notifyIntervalMs(10)      // TODO have some place to set this.
{
    qRegisterMetaType<FrequencySpectrum>("FrequencySpectrum");
    qRegisterMetaType<WindowFunction>("WindowFunction");
    CHECKED_CONNECT(&m_spectrumAnalyser,
                    SIGNAL(spectrumChanged(FrequencySpectrum)),
                    this,
                    SLOT(spectrumChanged(FrequencySpectrum)));
}

Engine::~Engine() { }

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

bool Engine::loadFile(const QString &fileName)
{
    ENGINE_DEBUG << fileName;
    m_audiofilename = fileName;
    reset();
    bool result = false;
    Q_ASSERT(!m_wavFileHandle);
    Q_ASSERT(!fileName.isEmpty());
    m_wavFileHandle = new WavFile(this);
    if (m_wavFileHandle->open(fileName)) {
        if (isPCMS16LE(m_wavFileHandle->fileFormat())) {
            result = initialize();
        } else {
            emit errorMessage(tr("Audio format not supported"),
                              formatToString(m_wavFileHandle->fileFormat()));
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
    qDebug() << bytesRead << bytesToRead << m_wavFileHandle->size() << m_wavFileHandle->headerLength();
    _qbuf.setBuffer(&m_buffer);
    _qbuf.open(QIODevice::ReadOnly);
    _qbuf.seek(0);
    emit bufferLengthChanged(bufferLength());
//    emit dataLengthChanged(dataLength());

    if (result) {
        // XXX why do we need both?
        m_analysisFile = new WavFile(this);
        m_analysisFile->open(fileName);
    }
    return result;
}

qint64 Engine::bufferLength() const
{
//    return m_wavFileHandle ? m_wavFileHandle->size() : -1;
    return _qbuf.buffer().size();
}

void Engine::setWindowFunction(WindowFunction type)
{
    m_spectrumAnalyser.setWindowFunction(type);
}

//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

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
        CHECKED_CONNECT(m_audioOutput, SIGNAL(notify()),
                        this, SLOT(audioNotify()));
        m_wavFileHandle->seek(0);
        m_bufferPosition = 0;
        _qbuf.seek(0);
        m_audioOutput->start(&_qbuf);
    }
}

void Engine::movePlaybackHead(double positionfraction)
{
    // Sets playback position to somewhere in middle of recording.
    // position == 0.0 means the beginning,
    // position == 1.0 means the end.

    qint64 byteposition = positionfraction * bufferLength();

    // Round off to start of the channel 0.
    byteposition -= byteposition % (m_format.channelCount() * m_format.sampleSize());
    playPositionChanged(byteposition);

//    bool result = m_wavFileHandle->seek(byteposition);
    bool result = _qbuf.seek(byteposition);
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
//-----------------------------------------------------------------------------

void Engine::audioNotify()
{
    _uSecs = m_audioOutput->processedUSecs();  // NUKEME
    // Find current playPosition in bytes
    const qint64 playPosition = _qbuf.pos();

    // Why would we be beyond the bufferLength???
    setPlayPosition(qMin(bufferLength(), playPosition));  // NUKEME

//   NUKEME  Q_ASSERT(m_wavFileHandle);

    // Look backwards from the playPosition when computing the spectrum
    const qint64 spectrumPosition = playPosition - m_spectrumBufferLength;

    const qint64 specWindowStart = qMax(qint64(0), spectrumPosition);
    const qint64 specWindowEnd   = qMin(_qbuf.size(), spectrumPosition + m_spectrumBufferLength);

    calculateSpectrum(specWindowStart, specWindowEnd);




    // ==============================
    // OKAY!
    // THIS IS IT!  ARE YOU READY?  It all happens here:

    _dfModel->evaluate();
    // TODO move elsewhere?  perhaps it's own timer loop?
    // Although, it wants to be on a timer synced with audio playback...
    // TA-DA !
    // ==============================
}

void Engine::audioStateChanged(QAudio::State state)
{
    ENGINE_DEBUG << "Engine::audioStateChanged from " << m_state << " to " << state;

    // Check error
    QAudio::Error error = QAudio::NoError;
    error = m_audioOutput->error();
    if (QAudio::NoError != error)
        ENGINE_DEBUG << "Engine::audioStateChanged [0] error " << error ;

    if (QAudio::IdleState == state && m_wavFileHandle && m_wavFileHandle->pos() == m_wavFileHandle->size()) {
        stopPlayback();
    } else {
        if (QAudio::StoppedState == state) {
            // Check error
            QAudio::Error error = QAudio::NoError;
            error = m_audioOutput->error();
            if (QAudio::NoError != error) {
                ENGINE_DEBUG << "Engine::audioStateChanged error " << error ;
                reset();
                return;
            }
        }

        // XXX workaround attempt for bug that causes audioNotify signals to stop
        // getting sent, even though music keeps playing and nothing else seems wrong.
        //CHECKED_CONNECT(m_audioOutput, SIGNAL(notify()), this, SLOT(audioNotify()));

        setState(state);
    }
}

void Engine::spectrumChanged(const FrequencySpectrum &spectrum)
{
    ENGINE_DEBUG << "Engine::spectrumChanged" << "pos" << m_spectrumPosition;
    emit spectrumChanged(m_spectrumPosition, m_spectrumBufferLength, spectrum);
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void Engine::resetAudioDevices()
{
    delete m_audioOutput;
    m_audioOutput = 0;
    setPlayPosition(0);
    m_spectrumPosition = 0;
}

void Engine::reset()
{
    stopPlayback();
    setState(QAudio::StoppedState);
    setFormat(QAudioFormat());
    delete m_wavFileHandle;
    m_wavFileHandle = 0;
    delete m_analysisFile;
    m_analysisFile = 0;
    m_buffer.clear();
    m_bufferPosition = 0;
//    m_dataLength = 0;
// NUKEME   emit dataLengthChanged(0); // XXX may want to emit songChanged()
    resetAudioDevices();
}

bool Engine::initialize()
{
    bool result = false;

    QAudioFormat format = m_format;

    if (selectFormat()) {
        resetAudioDevices();
        result = true;
        m_audioOutput = new QAudioOutput(m_audioOutputDevice, m_format, this);
        m_audioOutput->setNotifyInterval(m_notifyIntervalMs);
    } else {
        if (m_wavFileHandle)
            emit errorMessage(tr("Audio format not supported"), formatToString(m_format));
    }

    // (notify Interval may not be the same as set, if it's not supportable by device)
    if (m_notifyIntervalMs != m_audioOutput->notifyInterval()) {
        qDebug() << "Engine::initialize" << "requested notify interval" << m_notifyIntervalMs;
        qDebug() << "                  " << "actual    notify interval" << m_audioOutput->notifyInterval();
    }

//    ENGINE_DEBUG << "Engine::initialize" << "m_dataLength" << m_dataLength;
    ENGINE_DEBUG << "Engine::initialize" << "format" << m_format;

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
        // XXX are we sizing & clearing correctly???
        // Must copy data into spectrum buffer because Hahn window will be applied.
        // Note fromRawData uses implicit shared array.
        m_spectrumBuffer = QByteArray::fromRawData(m_buffer.constData() + start,
                                                   m_spectrumBufferLength);
        qint64 len = end-start+1;
        if (len < m_spectrumBufferLength) {
            qDebug() << Q_FUNC_INFO << "len" << len << "m_specbuflen" << m_spectrumBufferLength;
        }
        m_spectrumAnalyser.calculate(m_spectrumBuffer, m_format);
    }

}

void Engine::setFormat(const QAudioFormat &format)
{
    const bool changed = (format != m_format);
    m_format = format;
    m_spectrumBufferLength = SpectrumLengthSamples *
                            (m_format.sampleSize() / 8) * m_format.channelCount();
    if (changed)
        emit formatChanged(m_format);
}

/*
 * Sets interval at which spectrum is updated.
 * TODO:  this is also the interval at which the whole floor is
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
    qint64 positionInBytes = _qbuf.pos();
    return audioDuration(m_format, positionInBytes);
}
