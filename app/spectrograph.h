#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include "frequencyspectrum.h"
#include "Subrange.h"
#include "sublevel.h"
#include "SublevelNode.h"

#include <QWidget>
#include <QRubberBand>

/**
 * Widget which displays a spectrograph showing the frequency spectrum
 * of the window of audio samples most recently analyzed by the Engine.
 */
class Spectrograph : public QWidget
{
    Q_OBJECT

public:
    explicit Spectrograph(QWidget *parent = 0);
    ~Spectrograph();

    void setParams(int numBars, qreal lowFreq, qreal highFreq);

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    int numBars() { return m_bars.count(); }
    qreal freqLo() { return m_lowFreq; }
    qreal freqHi() { return m_highFreq; }

signals:
    void infoMessage(const QString &message, int intervalMs);
    void subrangeHasChanged(Subrange *subrange);

public slots:
    void reset();
    void spectrumChanged(const FrequencySpectrum &spectrum);
    void setNumBars(int numBars);
    void setFreqLo(int val);
    void setFreqHi(int val);
    void printSpectrum();
    void submeterSelectionChanged(SublevelNode *chosen);
    void displayThisSubrange(Subrange *subrange);

private:
    int barIndex(qreal frequency) const;
    int barIndex(qreal frequency, bool logspace) const;
    int barIndexLog(qreal frequency) const;
    QPair<qreal, qreal> barRange(int barIndex) const;
    QPair<qreal, qreal> barRange(int barIndex, bool logspace) const;
    QPair<qreal, qreal> barRangeLog(int barIndex) const;
    void updateBars();

protected:
    qreal frac2freq(qreal frac) const;

private:
    struct Bar {
        Bar() : value(0.0), nsamples(0), clipped(false) { }
        qreal   value;
        int   	nsamples;
        bool    clipped;
    };

    QVector<Bar>        m_bars;

    // Range of frequencies to display
    qreal               m_lowFreq;
    qreal               m_highFreq;

    // Set by engine
    FrequencySpectrum   m_spectrum;

    // For debugging
    bool   				m_printspectrum;

    // For dragging out subregions
    bool   				m_isDragging;
    QPoint				m_dragStart;
    QRubberBand*		m_rubberBand;

    SublevelNode *      _selectedSublevelNode;
    bool                _showSubrange;
    Subrange            _subrange;

};

#endif // SPECTROGRAPH_H
