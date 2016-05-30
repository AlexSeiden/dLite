#include <QAudioFormat>
#include "engine/utils.h"
#include "engine/dispatch.h"

// Returns position in microseconds, given position in bytes
qint64 audioDuration(const QAudioFormat &format, qint64 bytes)
{
    return (bytes * 1000000) /
        (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8));
}

// Returns position in bytes, given position in microSeconds.
qint64 audioLength(const QAudioFormat &format, qint64 microSeconds)
{
    // format.sampleRate() is in Hz, format.sampleSize() in bits
   qint64 result = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
       * microSeconds / 1000000;
   // Round off to start of the channel 0.
   result -= result % (format.channelCount() * format.sampleSize());
   return result;
}

qreal nyquistFrequency(const QAudioFormat &format)
{
    return format.sampleRate() / 2;
}

bool isPCM(const QAudioFormat &format)
{
    return (format.codec() == "audio/pcm");
}


bool isPCMS16LE(const QAudioFormat &format)
{
    return isPCM(format) &&
           format.sampleType() == QAudioFormat::SignedInt &&
           format.sampleSize() == 16 &&
           format.byteOrder() == QAudioFormat::LittleEndian;
}

const qint16  PCMS16MaxValue     =  32767;
const quint16 PCMS16MaxAmplitude =  32768; // because minimum is -32768

qreal pcmToReal(qint16 pcm)
{
    // Note this is linear
    return qreal(pcm) / PCMS16MaxAmplitude;
}

qint16 realToPcm(qreal real)
{
    return real * PCMS16MaxValue;
}

double clamp(double min, double max, double val)
{
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

int samples2ms(int samples)
{
    return samples * 1000. / Dispatch::Singleton()->getEngine()->format().sampleRate();
}

int bytes2ms(int bytes)
{
    return audioDuration(Dispatch::Singleton()->getEngine()->format(), bytes)/1000;
}
