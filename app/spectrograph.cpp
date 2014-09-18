#include "spectrograph.h"
#include "GuiSettings.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimerEvent>
#include <math.h>

Spectrograph::Spectrograph(QWidget *parent)
    :   QWidget(parent)
    ,   m_lowFreq(10.0)
    ,   m_highFreq(1000.0)
    ,   m_printspectrum(false)
    ,   m_isDragging(false)
    ,   m_dragStart(QPoint(0,0))
    ,   m_rubberBand(NULL)
    ,   _selectedSublevelNode(nullptr)
    ,   _showSubrange(false)
{
    setMinimumHeight(100);
}

Spectrograph::~Spectrograph() { }

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


// -----------------------------------------------------------------------------
// paintEvent

void Spectrograph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), GuiSettings::sg_bg);

    const int numBars = m_bars.count();

    // Draw the outline
    QPen gridPen(GuiSettings::sg_gridColor);
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

    if (numBars == 0)   // Ummm, something is seriously wrong if numBars==0
        return;

    // Calculate width of bars and gaps
    const int widgetWidth = rect().width();
    const int barPlusGapWidth = widgetWidth / numBars;
    const int barWidth = 0.8 * barPlusGapWidth;
    const int gapWidth = barPlusGapWidth - barWidth;
    const int paddingWidth = widgetWidth - numBars * (barWidth + gapWidth);
    const int leftPaddingWidth = (paddingWidth + gapWidth) / 2;
    const int barHeight = rect().height() - 2 * gapWidth;

    // Draw vertical lines between bars
    line = QLine(rect().topLeft(), rect().bottomLeft());
    line.translate(leftPaddingWidth/2, 0);
    for (int i=1; i<numBars; ++i) {
        line.translate(barPlusGapWidth, 0);
        painter.drawLine(line);
    }

    QColor lineColor = GuiSettings::sg_lineColor;
    lineColor.setAlphaF(0.75);
    QColor clipColor = GuiSettings::sg_clipColor;
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

        QColor color = GuiSettings::sg_barColor;
        if (m_bars[i].clipped)
            color = clipColor;

        painter.fillRect(bar, color);
    }

    // Label each bar with its frequency in Hz:
    painter.setPen(GuiSettings::sg_textColor);
    for (int i=0; i<numBars; ++i) {
        QRect textrect = rect();
        textrect.setLeft(rect().left() + leftPaddingWidth/2 + (i * (gapWidth + barWidth)));
        textrect.setWidth(barWidth+gapWidth);
        textrect.setBottom(rect().bottom() - GuiSettings::sg_textOffset);
        textrect.setTop(rect().bottom() - GuiSettings::sg_textOffset - GuiSettings::sg_textHeight);

        QPair<qreal, qreal> freqrange = barRange(i, true);
        int centerfreq = (freqrange.first + freqrange.second)/2.0;
        painter.drawText(textrect, Qt::AlignBottom|Qt::AlignHCenter,
                         QString::number(centerfreq));
    }

    // Do we have a subrange that should be drawn?
    if (_showSubrange) {
        QRectF subrangewindow = _subrange.getWindow();
        QRectF subwin;
        subwin.setTop   (rect().height() * subrangewindow.top());
        subwin.setBottom(rect().height() * subrangewindow.bottom());
        subwin.setLeft  (rect().width()  * subrangewindow.left());
        subwin.setRight (rect().width()  * subrangewindow.right());

        QPen pen(GuiSettings::sg_sublevelRegion);
        pen.setWidth(GuiSettings::sg_sublevelPenwidth);
        painter.setPen(pen);

        painter.drawRect(subwin);
    }
}

// -----------------------------------------------------------------------------
// mouse events
// These support dragging out a subrange in the graph.

//  mousePressEvent:
//      We're starting to drag out a subrange....
void Spectrograph::mousePressEvent(QMouseEvent *event)
{
    m_dragStart = event->pos();
    if (!m_rubberBand)
        m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    m_rubberBand->setGeometry(QRect(m_dragStart, QSize()));
    m_rubberBand->show();
}

//  mouseMoveEvent:
//      ...still dragging:  update rubber band lines....
void Spectrograph::mouseMoveEvent(QMouseEvent *event)
{
    m_rubberBand->setGeometry(QRect(m_dragStart, event->pos()).normalized());
}

//  mouseReleaseEvent:
//      ...done dragging:  send out new subrange.
void Spectrograph::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    QRect geo = m_rubberBand->geometry();
    // Clip geo to the spectrograph window.
    geo = geo.intersected(rect());
    QRectF subrect;

    // Find sampling window from the rubberband rect
    float fmin = float(geo.left())/rect().width();
    subrect.setLeft(fmin);  // For drawing
    fmin = frac2freq(fmin);     // Actual frequency

    float fmax = float(geo.right())/rect().width();
    subrect.setRight(fmax);
    fmax = frac2freq(fmax);

    float amin = float(geo.bottom())/rect().height();
    float amax = float(geo.top())/rect().height();
    subrect.setBottom(amin);
    subrect.setTop(amax);

    // Invert range, since window y increases from top to bottom.
    amin = 1.0 - amin;
    amax = 1.0 - amax;

    _subrange.setMinMax(fmin, fmax, amin, amax);
    _subrange.setWin(subrect);

    // Make sure appropriate SublevelNode & SublevelNodeItem are listening!
    emit subrangeHasChanged(&_subrange);
//    if (_selectedSublevelNode)
//        _selectedSublevelNode->setSubrange(_subrange);

    // Cleanup & redraw.
    m_rubberBand->hide();
    update();
}

void Spectrograph::reset()
{
    m_spectrum.reset();
    m_isDragging = false;
    spectrumChanged(m_spectrum);
}

// spectrumChanged
//      The new data is here!  The new data is here!  Wooooo!
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
    // Note: oddly, it doesn't matter what base you use for the logarhythms
    // TODO precompute these
    const qreal log_min = log10(m_lowFreq);
    const qreal log_max = log10(m_highFreq);
    const qreal delta_log = log_max - log_min;

    const qreal log_freq = log_min + frac*delta_log;
    const qreal freq = pow(10,log_freq);

    return (freq);
}

/*
 * updateBars()
 *
 * Loops over the entire spectrum in m_spectrum, which is supplied by the engine.
 * Computes values for each of the bars displayed in the spectrograph.
 */
void Spectrograph::updateBars()
{
    // init the vector of bars with empty bars
    m_bars.fill(Bar());

    // loop over all frequencies in the spectrum, and set the value
    FrequencySpectrum::const_iterator i = m_spectrum.begin();
    const FrequencySpectrum::const_iterator end = m_spectrum.end();
    int barindex;
    int index = 0;
    // TODO:  at least half the bars wasted--unused
    // Also, could optimize since they are monotonicly increasing

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
        // We've printed it above, so clear flag
        m_printspectrum = false;
    }
    update();
}

void Spectrograph::printSpectrum() {
    // print next time it's calculated.  then it'll be cleared.
    // for debugging.
    m_printspectrum = true;
}

void Spectrograph::submeterSelectionChanged(SublevelNode *chosen)
{
    _selectedSublevelNode = chosen;
    displayThisSubrange(chosen->getSubrange());
}

void Spectrograph::displayThisSubrange(Subrange *subrange)
{
    if (subrange) {
        _subrange = *subrange;
        _showSubrange = true;
    }
    else
        _showSubrange = false;

    update();
}

#if 0
// TODO find a home for this, either in settings or spectrograph
QLayout *createSpectrumOptionsUI(QWidget *parent, Spectrograph *spectrograph)
{

    QHBoxLayout *layout = new QHBoxLayout;

    // Options layout
    QLabel *numBandsLabel = new QLabel(tr("Number of Bands"), parent);
    QSpinBox *numBandsSpinBox = new QSpinBox(parent);
    numBandsSpinBox->setMinimum(3);
    numBandsSpinBox->setMaximum(40);
    numBandsSpinBox->setFixedWidth(50);
    numBandsSpinBox->setValue(spectrograph->numBars());
    QScopedPointer<QHBoxLayout> numBandsLayout (new QHBoxLayout);
    numBandsLayout->addWidget(numBandsLabel);
    numBandsLayout->addWidget(numBandsSpinBox);
    numBandsLayout->addStretch();
    layout->addLayout(numBandsLayout.data());
    numBandsLayout.take();

    QLabel *specMinLabel = new QLabel(tr("Min Freq"), parent);
    QSpinBox *specMinSpinBox = new QSpinBox(parent);
    specMinSpinBox->setMinimum(0);
    specMinSpinBox->setMaximum(20000);
    specMinSpinBox->setFixedWidth(70);
    specMinSpinBox->setValue(spectrograph->freqLo());
    QScopedPointer<QHBoxLayout> specMinLayout (new QHBoxLayout);
    specMinLayout->addWidget(specMinLabel);
    specMinLayout->addWidget(specMinSpinBox);
    specMinLayout->addStretch();
    layout->addLayout(specMinLayout.data());
    specMinLayout.take();

    QLabel *specMaxLabel = new QLabel(tr("Max Freq"), parent);
    QSpinBox *specMaxSpinBox = new QSpinBox(parent);
    specMaxSpinBox->setMinimum(0);
    specMaxSpinBox->setMaximum(40000);
    specMaxSpinBox->setFixedWidth(70);
    specMaxSpinBox->setValue(spectrograph->freqHi());
    QScopedPointer<QHBoxLayout> specMaxLayout (new QHBoxLayout);
    specMaxLayout->addWidget(specMaxLabel);
    specMaxLayout->addWidget(specMaxSpinBox);
    specMaxLayout->addStretch();
    layout->addLayout(specMaxLayout.data());
    specMaxLayout.take(); // ownership transferred to dialogLayout

    // TODO Move to Spectrograph, or settings dialog
    CHECKED_CONNECT(numBandsSpinBox, SIGNAL(valueChanged(int)),
            spectrograph, SLOT(setNumBars(int)));

    // TODO these probably shouldn't be spinboxes; or at least should increment
    // geometrically.
    CHECKED_CONNECT(specMinSpinBox, SIGNAL(valueChanged(int)),
            spectrograph, SLOT(setFreqLo(int)));

    CHECKED_CONNECT(specMaxSpinBox, SIGNAL(valueChanged(int)),
            spectrograph, SLOT(setFreqHi(int)));

    return layout;
}
#endif
