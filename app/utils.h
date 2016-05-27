#ifndef UTILS_H
#define UTILS_H

#include <QtCore/qglobal.h>
#include <QDebug>

QT_FORWARD_DECLARE_CLASS(QAudioFormat)

//-----------------------------------------------------------------------------
// Miscellaneous utility functions
//-----------------------------------------------------------------------------

qint64 audioDuration(const QAudioFormat &format, qint64 bytes);
qint64 audioLength(const QAudioFormat &format, qint64 microSeconds);

int samples2ms(int samples);
int bytes2ms(int bytes);

qreal nyquistFrequency(const QAudioFormat &format);

qreal pcmToReal(qint16 pcm);        // Scale PCM value to [-1.0, 1.0]

qint16 realToPcm(qreal real);       // Scale real value in [-1.0, 1.0] to PCM

// Check whether the audio format is PCM
bool isPCM(const QAudioFormat &format);

// Check whether the audio format is signed, little-endian, 16-bit PCM
bool isPCMS16LE(const QAudioFormat &format);

double clamp(double min, double max, double val);

//-----------------------------------------------------------------------------
// Debug output
//-----------------------------------------------------------------------------

class NullDebug
{
public:
    template <typename T>
    NullDebug& operator<<(const T&) { return *this; }
};

inline NullDebug nullDebug() { return NullDebug(); }

#ifdef LOG_ENGINE
#   define ENGINE_DEBUG qDebug()
#else
#   define ENGINE_DEBUG nullDebug()
#endif

#ifdef LOG_SPECTRUMANALYSER
#   define SPECTRUMANALYSER_DEBUG qDebug()
#else
#   define SPECTRUMANALYSER_DEBUG nullDebug()
#endif

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

// Macro which connects a signal to a slot, and which causes application to
// abort if the connection fails.  This is intended to catch programming errors
// such as mis-typing a signal or slot name.  It is necessary to write our own
// macro to do this - the following idiom
//     Q_ASSERT(connect(source, signal, receiver, slot));
// will not work because Q_ASSERT compiles to a no-op in release builds.

#define CHECKED_CONNECT(source, signal, receiver, slot) \
    if (!connect(source, signal, receiver, slot)) \
        qt_assert_x(Q_FUNC_INFO, "CHECKED_CONNECT failed", __FILE__, __LINE__);

#endif // UTILS_H
