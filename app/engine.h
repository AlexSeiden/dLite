#ifndef ENGINE_H
#define ENGINE_H

#include "spectrum.h"
#include "spectrumanalyser.h"
#include "wavfile.h"
#include "Cue.h"

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QObject>
#include <QVector>

class FrequencySpectrum;
class Dancefloor;
QT_BEGIN_NAMESPACE
class QAudioInput;
class QAudioOutput;
QT_END_NAMESPACE

/*
 * This class interfaces with the Qt Multimedia audio classes, and also with
 * the SpectrumAnalyser class.  Its role is to manage the capture and playback
 * of audio data, meanwhile performing real-time analysis of the audio level
 * and frequency spectrum.
 */
class Engine : public QObject
{
    friend class Cupid;

    Q_OBJECT

public:
    explicit Engine(QObject *parent = 0);
    ~Engine();

    QAudio::State state() const { return m_state; }
    int interval() {return m_notifyIntervalMs;}

    // return Current audio format
    // note May be QAudioFormat() if engine is not initialized
    const QAudioFormat& format() const { return m_format; }

    // Stop any ongoing recording or playback, and reset to ground state.
    void reset();

    // Load data from WAV file
    bool loadFile(const QString &fileName);

    // RMS level of the most recently processed set of audio samples.
    // \return Level in range (0.0, 1.0)
    qreal rmsLevel() const { return m_rmsLevel; }

    // Peak level of the most recently processed set of audio samples.
    // \return Level in range (0.0, 1.0)
    qreal peakLevel() const { return m_peakLevel; }

    // Position of the audio output device.
    // \return Position in bytes.
    qint64 playPosition() const { return m_playPosition; }

    // Position of the audio output device.
    // \return Position in microseconds.
    qint64 getCurrentTime() const {return _uSecs; }

    // Length of the internal engine buffer.
    // \return Buffer length in bytes.
    qint64 bufferLength() const;

    // Amount of data held in the buffer.
    // \return Data length in bytes.
    qint64 dataLength() const { return m_dataLength; }

    // Set window function applied to audio data before spectral analysis.
    void setWindowFunction(WindowFunction type);

    void setDancefloormodel(Dancefloor *df) {_dfModel = df;}

public slots:
    void startPlayback();
    void suspend();
    // Set update interval
    void setInterval(int val);

signals:
    void stateChanged(QAudio::State state);

    // Informational message for non-modal display
    void infoMessage(const QString &message, int durationMs);

    // Error message for modal display
    void errorMessage(const QString &heading, const QString &detail);

    // Format of audio data has changed
    void formatChanged(const QAudioFormat &format);

    // Length of buffer has changed.
    // \param duration Duration in microseconds
    void bufferLengthChanged(qint64 duration);

    // Amount of data in buffer has changed.
    // \param Length of data in bytes
    void dataLengthChanged(qint64 duration);

    // Position of the audio output device has changed.
    // \param position Position in bytes
    void playPositionChanged(qint64 position);

    // Level changed
    // \param rmsLevel RMS level in range 0.0 - 1.0
    // \param peakLevel Peak level in range 0.0 - 1.0
    // \param numSamples Number of audio samples analyzed
    void levelChanged(qreal rmsLevel, qreal peakLevel, int numSamples);

    // Spectrum has changed.
    // \param position Position of start of window in bytes
    // \param length   Length of window in bytes
    // \param spectrum Resulting frequency spectrum
    // TODO change this to only pass the spectrum
    void spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum);

private slots:
    void audioNotify();
    void audioStateChanged(QAudio::State state);
    void spectrumChanged(const FrequencySpectrum &spectrum);

private:
    void resetAudioDevices();
    bool initialize();
    bool selectFormat();
    void stopPlayback();
    void setState(QAudio::State state);
    void setFormat(const QAudioFormat &format);
    void setPlayPosition(qint64 position, bool forceEmit = false);
    void calculateLevel(qint64 position, qint64 length);
    void calculateSpectrum(qint64 position);

private:
    QAudio::State       m_state;

    WavFile*            m_wavFileHandle;
    // We need a second file handle via which to read data into m_buffer for analysis
    WavFile*            m_analysisFile;

    QAudioFormat        m_format;

    QAudioDeviceInfo    m_audioOutputDevice;
    QAudioOutput*       m_audioOutput;
    qint64              m_playPosition;

    QByteArray          m_buffer;
    qint64              m_bufferPosition;
    qint64              m_bufferLength;
    qint64              m_dataLength;

    int                 m_levelBufferLength;
    qreal               m_rmsLevel;
    qreal               m_peakLevel;

    int                 m_spectrumBufferLength;	// in bytes
    QByteArray          m_spectrumBuffer;
    SpectrumAnalyser    m_spectrumAnalyser;
    qint64              m_spectrumPosition;

    // Interval in milliseconds between calls that update the spectrum, etc.
    int    				m_notifyIntervalMs;
    Dancefloor     *_dfModel;

protected:
    // Used directly by Cupid.
    qint64              _uSecs;
    QString             m_audiofilename;
    QString             m_onsetfilename;

};

#endif // ENGINE_H
