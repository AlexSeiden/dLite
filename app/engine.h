#ifndef ENGINE_H
#define ENGINE_H

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QObject>
#include <QVector>
#include <QTimer>

#include "spectrum.h"
#include "spectrumanalyser.h"
#include "wavfile.h"

class FrequencySpectrum;
class Dancefloor;

QT_BEGIN_NAMESPACE
class QAudioOutput;
QT_END_NAMESPACE

/*
 * This class interfaces with the Qt Multimedia audio classes, and
 * the SpectrumAnalyser class.  Its manages audio playback
 * and sends data to the SpectrumAnalyser to calculate the
 * frequency spectrum.
 * It calls the Dancefloor when it's time to evaluate.
 */
class Engine : public QObject
{

    Q_OBJECT

public:
    explicit Engine(QObject *parent = 0);
    ~Engine();

    QAudio::State state() const { return m_state; }
    int interval() {return m_notifyIntervalMs;}

    // Return current audio format
    // Note: May be invalid object if engine is not initialized
    const QAudioFormat& format() const { return m_format; }

    // Stop playback and reset to ground state.
    void reset();

    // Load data from WAV file
    bool loadSong(const QString &fileName);

    qint64 getCurrentTime() const;  // Playback position in microseconds.
    QString getAudioFilename() const {return m_audiofilename;}

    qint64 bufferLength() const;    // Length of the song in bytes.
    qint64 bufferLengthMS() const;  // Length in milliseconds

    // Set window function applied to audio data before spectral analysis.
    void setWindowFunction(WindowFunction type);
    void setDancefloormodel(Dancefloor *df) {m_dfModel = df;}

public slots:
    void startPlayback();
    void suspend();
    void togglePlayback();
    void rewind();
    void movePlaybackHead(double positionfraction);
    void setUpdateInterval(int val);

signals:
    void stateChanged(QAudio::State state);

    // Informational message for non-modal display
    void infoMessage(const QString &message, int durationMs);

    // Error message for modal display
    void errorMessage(const QString &heading, const QString &detail);

    // Format of audio data has changed
    void formatChanged(const QAudioFormat &format);

    // Length of buffer has changed.
    // duration is in microseconds.
    void bufferLengthChanged(qint64 duration);

    void newSong(QString songfile);

    // Position of the audio output device has changed.
    // \param position Position in bytes
    void playPositionChanged(qint64 position);

    // Spectrum has changed.
    //  position Position of start of window in bytes
    //  length   Length of window in byte
    //  spectrum Resulting frequency spectrum
    // TODO change this to only pass the spectrum
    void spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum);

private slots:
    void timerNotify();
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
    void calculateSpectrum(qint64 start, qint64 end);

private:
    QAudio::State       m_state;

    WavFile*            m_wavFileHandle;
    QAudioFormat        m_format;

    QAudioDeviceInfo    m_audioOutputDevice;
    QAudioOutput*       m_audioOutput;
    qint64              m_playPosition;

    QByteArray          m_buffer;
    QBuffer             m_qbuf;  // TODO better name for qbuf

    int                 m_spectrumBufferLength;	// in bytes
    QByteArray          m_spectrumBuffer;
    SpectrumAnalyser    m_spectrumAnalyser;

    // Interval in milliseconds between calls that update the spectrum, etc.
    int    				m_notifyIntervalMs;

    //  Style : dfModel only needed to call evaluate.  Perhaps should use signals?
    Dancefloor*         m_dfModel;

    QString             m_audiofilename;
    QTimer*             m_timer;
};

#endif // ENGINE_H
