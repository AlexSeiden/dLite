#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include <QWidget>
#include <QRubberBand>
#include <QSet>

#include "engine/frequencyspectrum.h"
#include "engine/subrange.h"
#include "nodes/SublevelNode.h"

// Widget which displays a spectrograph showing the frequency spectrum
// of the window of audio samples most recently analyzed by the Engine.
class Spectrograph : public QWidget
{
    Q_OBJECT

public:
    explicit Spectrograph(QWidget *parent = 0);
    ~Spectrograph();

    void    setParams(int numBars, qreal lowFreq, qreal highFreq);

    void    paintEvent(QPaintEvent *event);
    void    mousePressEvent(QMouseEvent *event);
    void    mouseMoveEvent(QMouseEvent *event);
    void    mouseReleaseEvent(QMouseEvent *event);

    int     numBars() { return m_bars.count(); }
    qreal   freqLo() { return m_lowFreq; }
    qreal   freqHi() { return m_highFreq; }

signals:
    void    infoMessage(const QString &message, int intervalMs);

public slots:
    void    reset();
    void    spectrumChanged(const FrequencySpectrum &spectrum);
    void    setNumBars(int numBars);
    void    printSpectrum();
    void    submeterSelected(SublevelNode *chosen);
    void    submeterDeselected(SublevelNode *chosen);

private:
    int     barIndexLog(qreal frequency) const;
    QPair<qreal, qreal> barRangeLog(int barIndex) const;
    void    updateBars();
    bool    showSubrange();
    QRectF  getSubrangeWindow();
    void    setSubrangeWindow(Subrange &subr);

protected:
    double  frac2freq(qreal frac) const;
    double  freq2frac(qreal freq) const;
    QRectF  computeWinFromRange(Subrange *subrange);

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
    QPoint				m_dragStart;
    QRubberBand*		m_rubberBand;

    QSet<SublevelNode*> m_selectedSublevels;
    QRectF              m_subrangeRect;

};

#endif // SPECTROGRAPH_H
