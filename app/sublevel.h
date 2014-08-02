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
 * RMS level of the window of audio samples most recently analyzed
 * by the Engine.
 * Also has a pulsing display at the top, proportional to rms level
 */
class SublevelMeter : public QWidget
{
    Q_OBJECT

public:
    explicit SublevelMeter(QWidget *parent = 0);
    ~SublevelMeter();

    // QWidget
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setSelectable(bool status);
    bool setSelection(bool status);

signals:
    void iveBeenSelected(SublevelMeter *me);
    // TODO bad form to use custom type in selection?

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

    bool isSelectable;
    bool isSelected;
};

#endif // SUBLEVEL_H
