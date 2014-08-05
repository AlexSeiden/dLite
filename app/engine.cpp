#include "engine.h"
#include "utils.h"

#include <math.h>

#include <QAudioInput>
#include <QAudioOutput>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMetaObject>
#include <QSet>
#include <QThread>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Size of the level calculation window in microseconds
const int    LevelWindowUs          = 0.1 * 1000000;

//-----------------------------------------------------------------------------
// Constructor and destructor
//-----------------------------------------------------------------------------

Engine::Engine(QObject *parent)
    :   QObject(parent)
    ,   m_state(QAudio::StoppedState)
    ,   m_file(0)
    ,   m_analysisFile(0)
    ,   m_audioOutputDevice(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_audioOutput(0)
    ,   m_playPosition(0)
    ,   m_bufferPosition(0)
    ,   m_bufferLength(0)
    ,   m_dataLength(0)
    ,   m_levelBufferLength(0)
    ,   m_rmsLevel(0.0)
    ,   m_peakLevel(0.0)
    ,   m_spectrumBufferLength(0)
    ,   m_spectrumAnalyser()
    ,   m_spectrumPosition(0)
    ,   m_notifyIntervalMs(50)  // TODO MAKE THIS A CHANGABLE SETTING
{
    qRegisterMetaType<FrequencySpectrum>("FrequencySpectrum");
    qRegisterMetaType<WindowFunction>("WindowFunction");
    CHECKED_CONNECT(&m_spectrumAnalyser,
                    SIGNAL(spectrumChanged(FrequencySpectrum)),
                    this,
                    SLOT(spectrumChanged(FrequencySpectrum)));
}

Engine::~Engine()
{
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

bool Engine::loadFile(const QString &fileName)
{
    ENGINE_DEBUG << fileName;
    reset();
    bool result = false;
    Q_ASSERT(!m_file);
    Q_ASSERT(!fileName.isEmpty());
    m_file = new WavFile(this);
    if (m_file->open(fileName)) {
        if (isPCMS16LE(m_file->fileFormat())) {
            result = initialize();
        } else {
            emit errorMessage(tr("Audio format not supported"),
                              formatToString(m_file->fileFormat()));
        }
    } else {
        emit errorMessage(tr("Could not open file"), fileName);
    }
    if (result) {
        m_analysisFile = new WavFile(this);
        m_analysisFile->open(fileName);
    }
    return result;
}

// wtf?
qint64 Engine::bufferLength() const
{
    return m_file ? m_file->size() : m_bufferLength;
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
        CHECKED_CONNECT(m_audioOutput, SIGNAL(notify()),
                        this, SLOT(auxNotify()));
        if (m_file) {
            m_file->seek(0);
            m_bufferPosition = 0;
            m_dataLength = 0;
            m_audioOutput->start(m_file);
        } else {
            m_audioOutputIODevice.close();
            m_audioOutputIODevice.setBuffer(&m_buffer);
            m_audioOutputIODevice.open(QIODevice::ReadOnly);
            m_audioOutput->start(&m_audioOutputIODevice);
        }
    }
}

void Engine::suspend()
{
    if (QAudio::ActiveState == m_state || QAudio::IdleState == m_state) {
            m_audioOutput->suspend();
    }
}

//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void Engine::auxNotify()
{
    ENGINE_DEBUG << "Engine::auxNotify";
}

void Engine::audioNotify()
{
    // Find current playPosition in seconds
    const qint64 playPosition = audioLength(m_format, m_audioOutput->processedUSecs());
    setPlayPosition(qMin(bufferLength(), playPosition));

    // Look backwards from the playPosition when computing the level and the spectrum
    const qint64 levelPosition = playPosition - m_levelBufferLength;
    const qint64 spectrumPosition = playPosition - m_spectrumBufferLength;

    Q_ASSERT(m_file);

    if (levelPosition > m_bufferPosition || spectrumPosition > m_bufferPosition ||
        qMax(m_levelBufferLength, m_spectrumBufferLength) > m_dataLength) {
        m_bufferPosition = 0;
        m_dataLength = 0;
        // Data needs to be read into m_buffer in order to be analysed
        // TODO :-P gross.  should read into one buffer, and play from that buffer.
        const qint64 readPos = qMax(qint64(0), qMin(levelPosition, spectrumPosition));
        const qint64 readEnd = qMin(m_analysisFile->size(), qMax(levelPosition + m_levelBufferLength, spectrumPosition + m_spectrumBufferLength));
        const qint64 readLen = readEnd - readPos + audioLength(m_format, WaveformWindowDuration);
        ENGINE_DEBUG << "Engine::audioNotify [1]" << "analysisFileSize" << m_analysisFile->size() << "readPos" << readPos << "readLen" << readLen;

        if (m_analysisFile->seek(readPos + m_analysisFile->headerLength())) {
            m_buffer.resize(readLen);  // TODO wtf??? resize?? shouldn't this always be the same size? or at least, big enough?
            m_bufferPosition = readPos;
            m_dataLength = m_analysisFile->read(m_buffer.data(), readLen);
            ENGINE_DEBUG << "Engine::audioNotify [2]" << "bufferPosition" << m_bufferPosition << "dataLength" << m_dataLength;
        } else {
            ENGINE_DEBUG << "Engine::audioNotify [2]" << "file seek error";
        }

        emit bufferChanged(m_bufferPosition, m_dataLength, m_buffer);
    }
    if (levelPosition >= 0 && levelPosition + m_levelBufferLength < m_bufferPosition + m_dataLength)
        calculateLevel(levelPosition, m_levelBufferLength);
    if (spectrumPosition >= 0 && spectrumPosition + m_spectrumBufferLength < m_bufferPosition + m_dataLength)
        calculateSpectrum(spectrumPosition);
    else {
        // This seems to happen at the beginning of a song, where there isn't enough buffer to backwards to play
        ENGINE_DEBUG << "Engine::audioNotify [3]" << "buffer confusion" << "spectrumPosition:" << spectrumPosition
                        << "m_spectrumBufferLength "<< m_spectrumBufferLength
                        << "m_bufferPosition "<< m_bufferPosition
                        << "m_dataLength "<< m_dataLength;
    }
}

void Engine::audioStateChanged(QAudio::State state)
{
    ENGINE_DEBUG << "Engine::audioStateChanged from " << m_state << " to " << state;

    // Check error
    QAudio::Error error = QAudio::NoError;
    error = m_audioOutput->error();
    if (QAudio::NoError != error)
        ENGINE_DEBUG << "Engine::audioStateChanged [0] error " << error ;

    if (QAudio::IdleState == state && m_file && m_file->pos() == m_file->size()) {
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
        // Bug workaround???
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
    setLevel(0.0, 0.0, 0);
}

void Engine::reset()
{
    stopPlayback();
    setState(QAudio::StoppedState);
    setFormat(QAudioFormat());
    delete m_file;
    m_file = 0;
    delete m_analysisFile;
    m_analysisFile = 0;
    m_buffer.clear();
    m_bufferPosition = 0;
    m_bufferLength = 0;
    m_dataLength = 0;
    emit dataLengthChanged(0);
    resetAudioDevices();
}

bool Engine::initialize()
{
    bool result = false;

    QAudioFormat format = m_format;

    if (selectFormat()) {
        resetAudioDevices();
        emit bufferLengthChanged(bufferLength());
        emit dataLengthChanged(dataLength());
        emit bufferChanged(0, 0, m_buffer);
        result = true;
        m_audioOutput = new QAudioOutput(m_audioOutputDevice, m_format, this);
        m_audioOutput->setNotifyInterval(m_notifyIntervalMs);
    } else {
        if (m_file)
            emit errorMessage(tr("Audio format not supported"), formatToString(m_format));
    }

    ENGINE_DEBUG << "Engine::initialize" << "m_bufferLength" << m_bufferLength;
    ENGINE_DEBUG << "Engine::initialize" << "m_dataLength" << m_dataLength;
    ENGINE_DEBUG << "Engine::initialize" << "format" << m_format;
    // (notify Interval may not be the same as set, if it's not supportable by device)
    //ENGINE_DEBUG << "Engine::initialize" << "actual notify interval" << m_audioOutput->notifyInterval();

    return result;
}

bool Engine::selectFormat()
{
    bool foundSupportedFormat = false;

    if (m_file) {
        QAudioFormat format = m_format;
        // Header is read from the WAV file; just need to check whether
        // it is supported by the audio output device
        format = m_file->fileFormat();
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

void Engine::calculateLevel(qint64 position, qint64 length)
{
    Q_ASSERT(position + length <= m_bufferPosition + m_dataLength);

    qreal peakLevel = 0.0;
    qreal sum = 0.0;
    const char *ptr = m_buffer.constData() + position - m_bufferPosition;
    const char *const end = ptr + length;
    while (ptr < end) {
        const qint16 value = *reinterpret_cast<const qint16*>(ptr);
        const qreal fracValue = pcmToReal(value);
        peakLevel = qMax(peakLevel, fracValue);
        sum += fracValue * fracValue;
        ptr += 2;
    }
    const int numSamples = length / 2;
    qreal rmsLevel = sqrt(sum / numSamples);

    rmsLevel = qMax(qreal(0.0), rmsLevel);
    rmsLevel = qMin(qreal(1.0), rmsLevel);
    setLevel(rmsLevel, peakLevel, numSamples);

    ENGINE_DEBUG << "Engine::calculateLevel" << "pos" << position << "len" << length
                 << "rms" << rmsLevel << "peak" << peakLevel;
}

void Engine::calculateSpectrum(qint64 position)
{
    Q_ASSERT(position + m_spectrumBufferLength <= m_bufferPosition + m_dataLength);
    // Umm, I think this assertion is wrong; don't we want to assert that this is a power of two?
    Q_ASSERT(0 == m_spectrumBufferLength % 2); // constraint of FFT algorithm

    // QThread::currentThread is marked 'for internal use only', but
    // we're only using it for debug output here, so it's probably OK :)
    ENGINE_DEBUG << "Engine::calculateSpectrum" << QThread::currentThread()
                 << "pos" << position << "len" << m_spectrumBufferLength
                 << "spectrumAnalyser.isReady" << m_spectrumAnalyser.isReady();

    if (m_spectrumAnalyser.isReady()) {
        m_spectrumBuffer = QByteArray::fromRawData(m_buffer.constData() + position - m_bufferPosition,
                                                   m_spectrumBufferLength);
        m_spectrumPosition = position;
        m_spectrumAnalyser.calculate(m_spectrumBuffer, m_format);
    }
}

void Engine::setFormat(const QAudioFormat &format)
{
    const bool changed = (format != m_format);
    m_format = format;
    m_levelBufferLength = audioLength(m_format, LevelWindowUs);
    m_spectrumBufferLength = SpectrumLengthSamples *
                            (m_format.sampleSize() / 8) * m_format.channelCount();
    if (changed)
        emit formatChanged(m_format);
}

void Engine::setLevel(qreal rmsLevel, qreal peakLevel, int numSamples)
{
    m_rmsLevel = rmsLevel;
    m_peakLevel = peakLevel;
    emit levelChanged(m_rmsLevel, m_peakLevel, numSamples);
}

/**
 * Sets interval at which spectrum is updated.
 */
void Engine::setInterval(int val)
{
    m_notifyIntervalMs = val;
    m_audioOutput->setNotifyInterval(m_notifyIntervalMs);
}
