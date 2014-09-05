#ifndef SUBLEVEL_H
#define SUBLEVEL_H

#include <QTime>
#include <QWidget>
#include <QMimeData>
#include "frequencyspectrum.h"
#include "cue.h"
#include "Param.h"

// -----------------------------------------------------------------------------
// Subrange

class Subrange
{
    friend class Spectrograph;  // TODO remove this

protected:
    float freqMin, freqMax;
    float ampMin, ampMax;

    QRectF subrangeWindow;

public:
    Subrange();
    ~Subrange();

    double amplitudeWithinWindow(double amp);
    bool isFrequencyWithinWindow(double freq);
};



// -----------------------------------------------------------------------------
// Sublevel meter
/**
 * Widget which displays a vertical audio level meter, indicating the
 * RMS level of the window of audio samples most recently analyzed
 * by the Engine within a specfied frequency range and amplitude.
 * Also has a pulsing display at the top, proportional to rms level
 */
class SublevelMeter : public QWidget
{
    Q_OBJECT

    friend class Spectrograph; // TODO yuck defriend

public:
    explicit SublevelMeter(QWidget *parent = 0);
    ~SublevelMeter();

    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void setSelectable(bool status);
    bool setSelection(bool status);
    void calculateLevel();

    void setActive(bool status) { _active = status; }
    Subrange getRange() {return _range;}
    void setRange(Subrange &newrange) { _range = newrange; }

//    template<class PT> std::function<void(PT&)> createProviderClosure();
    std::function<void(float&)> createProviderClosure();


signals:
    void iveBeenSelected(SublevelMeter *me);
    void levelChanged(qreal level);

public slots:
    void spectrumChanged(qint64 position, qint64 length,
                         const FrequencySpectrum &spectrum);

private:
    // Set by engine
    FrequencySpectrum   m_spectrum;

    // Height of level bar.
    float  m_level;

    bool _active;
    bool _selectable;
    bool _selected;
    bool _dragTarget;

    Cue * _cue;


protected:
    Subrange    _range;
};

#endif // SUBLEVEL_H
