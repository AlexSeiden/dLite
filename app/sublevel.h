#ifndef SUBLEVEL_H
#define SUBLEVEL_H

#include <QTime>
#include <QWidget>
#include "frequencyspectrum.h"

// -----------------------------------------------------------------------------
// Subrange

class Subrange
{
    friend class Spectrograph;

protected:
    float freqMin, freqMax;
    float ampMin, ampMax;

    QRectF subrangeWindow;

public:
    Subrange();
    ~Subrange();

    double amplitudeWithinWindow(double amp);
    bool frequencyWithinWindow(double freq);
};



// -----------------------------------------------------------------------------
// Sublevel meter
/**
 * Widget which displays a vertical audio level meter, indicating the
 * RMS level of the window of audio samples most recently analyzed
 * by the Engine.
 * Also has a pulsing display at the top, proportional to rms level
 */
class SublevelMeter : public QWidget
{
    Q_OBJECT

    friend class Spectrograph;

public:
    explicit SublevelMeter(QWidget *parent = 0);
    ~SublevelMeter();

    // QWidget
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setActive(bool status);
    void setSelectable(bool status);
    bool setSelection(bool status);
    void updateSubsamples();

signals:
    void iveBeenSelected(SublevelMeter *me);
    // TODO bad form to use custom type in selection?

public slots:
    void reset();
    void levelChanged(qreal rmsLevel);
    void spectrumChanged(qint64 position, qint64 length,
                         const FrequencySpectrum &spectrum);

private:
    // Set by engine
    FrequencySpectrum   m_spectrum;

    // Height of RMS level bar.
    qreal m_rmsLevel;

    QColor m_rmsColor;
    QColor m_squareColor;

    bool _active;
    bool _selectable;
    bool _selected;

protected:
    Subrange    range;
};

#endif // SUBLEVEL_H
