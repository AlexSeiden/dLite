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
    ,   m_barSelected(NullIndex)
    ,   m_timerId(NullTimerId)
    ,   m_lowFreq(10.0)
    ,   m_highFreq(1000.0)
    ,   m_printspectrum(false)
    ,   m_isDragging(false)
    ,   m_dragStart(QPoint(0,0))
    ,   m_rubberBand(NULL)
    ,   subrangeMetering(false)
{
    setMinimumHeight(100);
}

Spectrograph::~Spectrograph()
{
}

void Spectrograph::setParams(int numBars, qreal lowFreq, qreal highFreq)
{
    // TODO sensable defaults rather than asserts
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

    // Draw the labels
    painter.setPen(QColor(230,230,255));
    const int textHeight = 50;
    const int bottomOffset = 10;
    for (int i=0; i<numBars; ++i) {
        QRect textrect = rect();
        textrect.setLeft(rect().left() + leftPaddingWidth/2 + (i * (gapWidth + barWidth)));
        textrect.setWidth(barWidth+gapWidth);
        textrect.setBottom(rect().bottom() - bottomOffset);
        textrect.setTop(rect().bottom() - bottomOffset - textHeight);

        QPair<qreal, qreal> freqrange = barRange(i, true);
        int centerfreq = (freqrange.first + freqrange.second)/2.0;
        painter.drawText(textrect, Qt::AlignBottom|Qt::AlignHCenter, QString::number(centerfreq));
    }

}

void Spectrograph::mousePressEvent(QMouseEvent *event)
{
    m_dragStart = event->pos();
    if (!m_rubberBand)
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    m_rubberBand->setGeometry(QRect(m_dragStart, QSize()));
    m_rubberBand->show();
}

void Spectrograph::mouseMoveEvent(QMouseEvent *event)
{
    m_rubberBand->setGeometry(QRect(m_dragStart, event->pos()).normalized());
}

void Spectrograph::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //m_rubberBand->hide();
    QRect geo = m_rubberBand->geometry();
    geo = geo.intersected(rect());
    // Find sampling window from rubberband rect
    subrangeMinfreq = float(geo.left())/rect().width();
    subrangeMinfreq = frac2freq(subrangeMinfreq);

    subrangeMaxfreq = float(geo.right())/rect().width();
    subrangeMaxfreq = frac2freq(subrangeMaxfreq);

    subrangeMinamp = 1.0 - float(geo.bottom())/rect().height();
    subrangeMaxamp = 1.0 - float(geo.top())/rect().height();

    subrangeMetering = true;

    const QString message = QString("(%1,%2) to (%3, %4)")
                                .arg(subrangeMinfreq)
                                .arg(subrangeMinamp)
                                .arg(subrangeMaxfreq)
                                .arg(subrangeMaxamp);
    emit infoMessage(message, 10000);

    // TODO emit update event

    // TODO emit samplewindow event

}

void Spectrograph::reset()
{
    m_spectrum.reset();
    m_isDragging = false;
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
    const qreal delta_log = log_max - log_min;
    const qreal step_log = delta_log/m_bars.count();

    const qreal log_freq = log10(frequency);
    const qreal log_index = (log_freq - log_min) / step_log;
    const int index = floor(log_index);

    if (index <0 || index >= m_bars.count()) {
        qDebug() << "frequency" << frequency << "index" << index <<"m_bars.count()" << m_bars.count();
        Q_ASSERT(false);
    }
    return index;
}

/*
 * Give the index of a bar, returns the range of frequencies it covers.
 */
QPair<qreal, qreal> Spectrograph::barRange(int index, bool logspace) const
{
    if (logspace)
        return barRangeLog(index);
    else
        return barRange(index);
}

QPair<qreal, qreal> Spectrograph::barRange(int index) const
{
    Q_ASSERT(index >= 0 && index < m_bars.count());
    const qreal bandWidth = (m_highFreq - m_lowFreq) / m_bars.count();
    return QPair<qreal, qreal>(index * bandWidth + m_lowFreq, (index+1) * bandWidth+m_lowFreq);
}

QPair<qreal, qreal> Spectrograph::barRangeLog(int index) const
{
    Q_ASSERT(index >= 0 && index < m_bars.count());

    // Note: it doesn't matter what base you use for the logarhythms
    const qreal log_min = log10(m_lowFreq);
    const qreal log_max = log10(m_highFreq);
    const qreal delta_log = log_max - log_min;
    const qreal step_log = delta_log/m_bars.count();
    const qreal log_freq_min = log_min + index*step_log;
    const qreal log_freq_max = log_freq_min + step_log;
    const qreal freq_min = pow(10,log_freq_min);
    const qreal freq_max = pow(10,log_freq_max);

    return QPair<qreal, qreal>(freq_min, freq_max);
}

qreal Spectrograph::frac2freq(qreal frac) const
{
    // Note: it doesn't matter what base you use for the logarhythms
    // TODO precompute these
    const qreal log_min = log10(m_lowFreq);
    const qreal log_max = log10(m_highFreq);
    const qreal delta_log = log_max - log_min;

    const qreal log_freq = log_min + frac*delta_log;
    const qreal freq = pow(10,log_freq);

    return (freq);
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
    // TODO:  at least half the bars wasted-- unused
    // Also, could optimize since they are monotonicly increasing
    Bar subrangeBar;

    for ( ; i != end; ++i, ++index) {
        const FrequencySpectrum::Element e = *i;
        if (m_printspectrum) {
            qDebug() << "sample" << index << e.frequency << e.amplitude;
        }
        if (e.frequency >= m_lowFreq && e.frequency < m_highFreq) {
            barindex = barIndex(e.frequency, true);
            Bar &bar = m_bars[barindex];
            bar.value += e.amplitude;
            bar.nsamples++;
            bar.clipped |= e.clipped;
        }
        // Do we have a window?
        if (subrangeMetering) {
            if (e.frequency >= subrangeMinfreq && e.frequency < subrangeMaxfreq) {
                // TODO amplitude window
                subrangeBar.value += e.amplitude;
                subrangeBar.nsamples++;
                subrangeBar.clipped |= e.clipped;
            }
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
    if (subrangeMetering) {
        if (subrangeBar.nsamples > 0)
            subrangeBar.value /= subrangeBar.nsamples;
        if (subrangeBar.value > 1.0) {
            subrangeBar.value = 1.0;
            subrangeBar.clipped = true;
        }
        emit subrangeLevelChanged(subrangeBar.value, subrangeBar.value, 256);
    }
    if (m_printspectrum) {
        m_printspectrum = false;
    }
    update();
}


#if 0
void Spectrograph::selectBar(int index) {
    const QPair<qreal, qreal> frequencyRange = barRange(index, true);
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
#endif

void Spectrograph::printSpectrum() {
    // print next time it's calculated.  then it'll be cleared.
    m_printspectrum = true;
}
