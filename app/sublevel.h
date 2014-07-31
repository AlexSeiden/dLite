#ifndef SUBLEVEL_H
#define SUBLEVEL_H

#include <QTime>
#include <QWidget>

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

/**
 * Widget which displays a vertical audio level meter, indicating the
 * RMS and peak levels of the window of audio samples most recently analyzed
 * by the Engine.
 */
class SublevelMeter : public QWidget
{
    Q_OBJECT

public:
    explicit SublevelMeter(QWidget *parent = 0);
    ~SublevelMeter();

    void paintEvent(QPaintEvent *event);

public slots:
    void reset();
    void levelChanged(qreal rmsLevel, qreal peakLevel, int numSamples);

private:
    /**
     * Height of RMS level bar.
     * Range 0.0 - 1.0.
     */
    qreal m_rmsLevel;

    QColor m_rmsColor;
    QColor m_squareColor;
};

#endif // SUBLEVEL_H
