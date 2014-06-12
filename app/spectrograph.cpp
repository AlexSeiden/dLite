#include "spectrograph.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimerEvent>
#include <math.h>

const int NullTimerId = -1;
const int NullIndex = -1;
const int BarSelectionInterval = 2000; // Duration a bar remains selected, in ms

Spectrograph::Spectrograph(QWidget *parent)
    :   QWidget(parent)
    ,   m_numBars(20)
    ,   m_barSelected(NullIndex)
    ,   m_timerId(NullTimerId)
    ,   m_lowFreq(0.0)
    ,   m_highFreq(1000.0)
    ,   m_printspectrum(false)
{
    setMinimumHeight(100);
}

Spectrograph::~Spectrograph()
{

}

void Spectrograph::setParams(int numBars, qreal lowFreq, qreal highFreq)
{
    Q_ASSERT(numBars > 0);
    Q_ASSERT(highFreq > lowFreq);
    m_bars.resize(numBars);
    m_lowFreq = lowFreq;
    m_highFreq = highFreq;
    updateBars();
}

void Spectrograph::setNumBars(int numBars)
{
    Q_ASSERT(numBars > 0);
    m_bars.resize(numBars);
    m_numBars = numBars;  // TODO get rid of this and just use m_bars.count()
    updateBars();
}

void Spectrograph::setFreqLo(int val)
{
    m_lowFreq = val; // Note implicit cast of int val to qreal m_lowFreq
    // TODO verify lo<hi
    updateBars();
}

void Spectrograph::setFreqHi(int val)
{
    m_highFreq = val; // Note implicit cast of int val to qreal m_highFreq
    // TODO verify lo<hi
    updateBars();
}

// Clear selected bar after timer expires
void Spectrograph::timerEvent(QTimerEvent *event)
{
    Q_ASSERT(event->timerId() == m_timerId);
    Q_UNUSED(event) // suppress warnings in release builds
    killTimer(m_timerId);
    m_timerId = NullTimerId;
    m_barSelected = NullIndex;
    update();
}

void Spectrograph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    const int numBars = m_bars.count();
    if (numBars != m_numBars)
        qDebug() << "m_numBars" << m_numBars << "count" << numBars;

    QColor barColor(51, 204, 102);
    QColor clipColor(255, 255, 0);

    // Draw the outline
    const QColor gridColor = barColor.darker();
    QPen gridPen(gridColor);
    painter.setPen(gridPen);
    painter.drawLine(rect().topLeft(), rect().topRight());
    painter.drawLine(rect().topRight(), rect().bottomRight());
    painter.drawLine(rect().bottomRight(), rect().bottomLeft());
    painter.drawLine(rect().bottomLeft(), rect().topLeft());

    QVector<qreal> dashes;
    dashes << 2 << 2;
    gridPen.setDashPattern(dashes);
    painter.setPen(gridPen);

    // Draw horizontal lines
    const int numVerticalSections = 10;
    QLine line(rect().topLeft(), rect().topRight());
    for (int i=1; i<numVerticalSections; ++i) {
        line.translate(0, rect().height()/numVerticalSections);
        painter.drawLine(line);
    }

    if (numBars == 0)
        return;

    // Calculate width of bars and gaps
    const int widgetWidth = rect().width();
    const int barPlusGapWidth = widgetWidth / numBars;
    const int barWidth = 0.8 * barPlusGapWidth;
    const int gapWidth = barPlusGapWidth - barWidth;
    const int paddingWidth = widgetWidth - numBars * (barWidth + gapWidth);
    const int leftPaddingWidth = (paddingWidth + gapWidth) / 2;
    const int barHeight = rect().height() - 2 * gapWidth;

    // Highlight region of selected bar
    if (m_barSelected != NullIndex) {
        QRect regionRect = rect();
        regionRect.setLeft(m_barSelected * rect().width() / numBars);
        regionRect.setWidth(rect().width() / numBars);
        QColor regionColor(202, 202, 64);
        painter.setBrush(Qt::DiagCrossPattern);
        painter.fillRect(regionRect, regionColor);
        painter.setBrush(Qt::NoBrush);
    }

    // Draw vertical lines between bars
    line = QLine(rect().topLeft(), rect().bottomLeft());
    line.translate(leftPaddingWidth/2, 0);
    for (int i=1; i<numBars; ++i) {
        line.translate(barPlusGapWidth, 0);
        painter.drawLine(line);
    }

    barColor = barColor.lighter();
    barColor.setAlphaF(0.75);
    clipColor.setAlphaF(0.75);

    // Draw the bars
    for (int i=0; i<numBars; ++i) {
        const qreal value = m_bars[i].value;
        Q_ASSERT(value >= 0.0 && value <= 1.0);
        QRect bar = rect();
        bar.setLeft(rect().left() + leftPaddingWidth + (i * (gapWidth + barWidth)));
        bar.setWidth(barWidth);
        bar.setTop(rect().top() + gapWidth + (1.0 - value) * barHeight);
        bar.setBottom(rect().bottom() - gapWidth);

        QColor color = barColor;
        if (m_bars[i].clipped)
            color = clipColor;

        painter.fillRect(bar, color);
    }
}

void Spectrograph::mousePressEvent(QMouseEvent *event)
{
    const QPoint pos = event->pos();
    const int index = m_bars.count() * (pos.x() - rect().left()) / rect().width();
    selectBar(index);
}

void Spectrograph::reset()
{
    m_spectrum.reset();
    spectrumChanged(m_spectrum);
}

void Spectrograph::spectrumChanged(const FrequencySpectrum &spectrum)
{
    m_spectrum = spectrum;
    updateBars();
}

/*
 * Given a frequency, returns the bar that covers it.
 */
int Spectrograph::barIndex(qreal frequency, bool logspace) const
{
    if (logspace)
        return barIndexLog(frequency);
    else
        return barIndex(frequency);
}

int Spectrograph::barIndex(qreal frequency) const
{
    Q_ASSERT(frequency >= m_lowFreq && frequency < m_highFreq);
    const qreal bandWidth = (m_highFreq - m_lowFreq) / m_bars.count();
    const int index = (frequency - m_lowFreq) / bandWidth;
    if (index <0 || index >= m_bars.count())
        Q_ASSERT(false);
    return index;
}

/*
 * Given a frequency, returns the bar that covers it.
 * IN LOG SPACE
 */
int Spectrograph::barIndexLog(qreal frequency) const
{
    Q_ASSERT(frequency >= m_lowFreq && frequency < m_highFreq);

    const qreal log_min = log10(m_lowFreq);
    const qreal log_max = log10(m_highFreq);
    const qreal log_freq = log10(frequency);
    const qreal delta_log = log_max - log_min;

    qreal log_index = (log_freq - log_min) / delta_log;
    const int index = floor(pow(10, log_index));

    if (index <0 || index >= m_bars.count())
        Q_ASSERT(false);
    return index;
}

/*
 * Give the index of a bar, returns the range of frequencies it covers.
 */
QPair<qreal, qreal> Spectrograph::barRange(int index) const
{
    Q_ASSERT(index >= 0 && index < m_bars.count());
    const qreal bandWidth = (m_highFreq - m_lowFreq) / m_bars.count();
    return QPair<qreal, qreal>(index * bandWidth + m_lowFreq, (index+1) * bandWidth);
}

void Spectrograph::updateBars()
{
    // init the vector of bars with empty bars
    m_bars.fill(Bar());

    // loop over all frequencies in the spectrum, and set the value
    FrequencySpectrum::const_iterator i = m_spectrum.begin();
    const FrequencySpectrum::const_iterator end = m_spectrum.end();
    int barindex;
    int index = 0;
    for ( ; i != end; ++i, ++index) {
        const FrequencySpectrum::Element e = *i;
        if (m_printspectrum) {
            qDebug() << "sample" << index << e.frequency << e.amplitude;
        }
        if (e.frequency >= m_lowFreq && e.frequency < m_highFreq) {
            barindex = barIndex(e.frequency);
            Bar &bar = m_bars[barindex];
#if 0
            // GAHHH SETS MAX VALUE FOR FREQS NOT TOTAL
            bar.value = qMax(bar.value, e.amplitude);
#else
            bar.value += e.amplitude;
            bar.nsamples++;
#endif
            bar.clipped |= e.clipped;
        }
    }

    // Normalize by number of samples
    QVector<Bar>::iterator j = m_bars.begin();
    const QVector<Bar>::const_iterator barsend = m_bars.end();
    for ( ; j != barsend; ++j) {
        Bar &bar = *j;
        if (bar.nsamples > 0)
            bar.value /= bar.nsamples;
        if (bar.value > 1.0) {
            bar.value = 1.0;
            bar.clipped = true;
        }
    }
    if (m_printspectrum) {
        m_printspectrum = false;
    }
    update();
}


void Spectrograph::selectBar(int index) {
    const QPair<qreal, qreal> frequencyRange = barRange(index);
    const QString message = QString("%1 - %2 Hz")
                                .arg(frequencyRange.first)
                                .arg(frequencyRange.second);
    emit infoMessage(message, BarSelectionInterval);

    if (NullTimerId != m_timerId)
        killTimer(m_timerId);
    m_timerId = startTimer(BarSelectionInterval);

    m_barSelected = index;
    update();
}

void Spectrograph::printSpectrum() {
    // print next time it's calculated.  then it'll be cleared.
    m_printspectrum = true;
}
