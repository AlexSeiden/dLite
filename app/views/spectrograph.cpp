// Copyright (c) 2014-2016 Alex Seiden
// Portions Copyright (C) Digia Plc as noted below.
//
/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**/

#include <math.h>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimerEvent>

#include "views/spectrograph.h"
#include "views/gui_settings.h"

Spectrograph::Spectrograph(QWidget *parent)
    :   QWidget(parent)
    ,   m_lowFreq(20.0)
    ,   m_highFreq(10000.0)
    ,   m_printspectrum(false)
    ,   m_dragStart(QPoint(0,0))
    ,   m_rubberBand(NULL)
{
    setMinimumHeight(300);
    setMinimumWidth(400);
    setNumBars(20);
}

Spectrograph::~Spectrograph() { }

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
    updateBars();
}


// -----------------------------------------------------------------------------
void Spectrograph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.save();
    painter.fillRect(rect(), guisettings->sg_bg);

    const int numBars = m_bars.count();

    // Draw the outline
    QPen gridPen(guisettings->sg_gridColor);
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

    Q_ASSERT(numBars > 0);

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

    QColor lineColor = guisettings->sg_lineColor;
    lineColor.setAlphaF(0.75);
    QColor clipColor = guisettings->sg_clipColor;
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

        QColor color = guisettings->sg_barColor;
        if (m_bars[i].clipped)
            color = clipColor;

        painter.fillRect(bar, color);
    }

    // Label each bar with its frequency in Hz:
    painter.setPen(guisettings->sg_textColor);
    for (int i=0; i<numBars; ++i) {
        QRect textrect = rect();
        textrect.setLeft(rect().left() + leftPaddingWidth/2 + (i * (gapWidth + barWidth)));
        textrect.setWidth(barWidth+gapWidth);
        textrect.setBottom(rect().bottom() - guisettings->sg_textOffset);
        textrect.setTop(rect().bottom() - guisettings->sg_textOffset - guisettings->sg_textHeight);

        QPair<qreal, qreal> freqrange = barRangeLog(i);
        int centerfreq = (freqrange.first + freqrange.second)/2.0;
        QString hz;
        if (centerfreq<1000)
            hz = QString::number(centerfreq);
        else
            hz = QString("%1K").arg(qreal(centerfreq)/1000,0,'f',1);
        painter.setFont(guisettings->sg_HzFont);
        painter.drawText(textrect, Qt::AlignBottom|Qt::AlignHCenter, hz);
    }

    // Do we have a subrange that should be drawn?
    if (showSubrange()) {
        QRectF subrangewindow = getSubrangeWindow();
        QRectF subwin;
        subwin.setTop   (rect().height() * subrangewindow.top());
        subwin.setBottom(rect().height() * subrangewindow.bottom());
        subwin.setLeft  (rect().width()  * subrangewindow.left());
        subwin.setRight (rect().width()  * subrangewindow.right());

        QPen pen(guisettings->sg_sublevelRegion);
        pen.setWidth(guisettings->sg_sublevelPenwidth);
        painter.setPen(pen);

        painter.drawRect(subwin);
    }
    painter.restore();
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

    Subrange subr;
    subr.setMinMax(fmin, fmax, amin, amax);

    setSubrangeWindow(subr);

    // Cleanup & redraw.
    m_rubberBand->hide();
    update();
}

void Spectrograph::reset()
{
    m_spectrum.reset();
    spectrumChanged(m_spectrum);
}

// spectrumChanged
//      The new data is here!  The new data is here!  Wooooo!
void Spectrograph::spectrumChanged(const FrequencySpectrum &spectrum)
{
    m_spectrum = spectrum;
    updateBars();
}

// -----------------------------------------------------------------------------
// Math & shit

// Given a frequency, returns the bar that covers it.
// Amplitude is in log space.
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

// Given the index of a bar, returns the range of frequencies it covers.
// Amplitude is in log space.
QPair<qreal, qreal> Spectrograph::barRangeLog(int index) const
{
    Q_ASSERT(index >= 0 && index < m_bars.count());

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

double Spectrograph::frac2freq(qreal frac) const
{
    const qreal log_min = log10(m_lowFreq);
    const qreal log_max = log10(m_highFreq);
    const qreal delta_log = log_max - log_min;

    const qreal log_freq = log_min + frac*delta_log;
    const qreal freq = pow(10,log_freq);

    return (freq);
}

double Spectrograph::freq2frac(double freq) const
{
    const double log_min = log10(m_lowFreq);
    const double log_max = log10(m_highFreq);
    const double log_freq = log10(freq);
    const double delta_log = log_max - log_min;

    return ((log_freq - log_min)/delta_log);
}

QRectF Spectrograph::computeWinFromRange(Subrange *subrange)
{
    // $$$ Could optimize--this is recomputing more often than needed.
    // Invert amplitudes, since drawing is y increases downwards.
    double ampMin = 1.0 - subrange->m_ampMin;
    double ampMax = 1.0 - subrange->m_ampMax;
    double minSubwindowFreq = freq2frac(subrange->m_freqMin);
    double maxSubwindowFreq = freq2frac(subrange->m_freqMax);

    m_subrangeRect.setCoords(minSubwindowFreq, ampMax, maxSubwindowFreq, ampMin);
    return m_subrangeRect;
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

    // Loop over all frequencies in the spectrum, and set the value
    FrequencySpectrum::const_iterator i = m_spectrum.begin();
    const FrequencySpectrum::const_iterator end = m_spectrum.end();
    int barindex;
    int index = 0;
    // OPT:  at least half the bars wasted--unused
    // Also, could optimize since they are monotonicly increasing

    for ( ; i != end; ++i, ++index) {
        const FrequencySpectrum::Element e = *i;
        if (m_printspectrum) {
            qDebug() << "sample" << index << e.m_frequency << e.m_amplitude;
        }
        if (e.m_frequency >= m_lowFreq && e.m_frequency < m_highFreq) {
            barindex = barIndexLog(e.m_frequency);
            Bar &bar = m_bars[barindex];
            bar.value += e.m_amplitude;
            bar.nsamples++;
            bar.clipped |= e.m_clipped;
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
    // Print next time it's calculated.  Then it'll be cleared.
    // for debugging.
    m_printspectrum = true;
}

void Spectrograph::submeterSelected(SublevelNode *chosen)
{
    m_selectedSublevels.insert(chosen);
    update();
}

void Spectrograph::submeterDeselected(SublevelNode *chosen)
{
    m_selectedSublevels.remove(chosen);
    update();
}

bool Spectrograph::showSubrange()
{
    if (m_selectedSublevels.size() == 1)
        return true;
    return false;
}

QRectF Spectrograph::getSubrangeWindow()
{
    // Caclulates the rect that should be drawn for a given range.
    if (m_selectedSublevels.size() == 1) {
        QSet<SublevelNode*>::const_iterator i = m_selectedSublevels.constBegin();
        return (computeWinFromRange((*i)->getSubrange()));
    }
    return QRectF();
}

void Spectrograph::setSubrangeWindow(Subrange &subrange)
{
    // Store a newly dragged out range into the subrange struct.
    if (m_selectedSublevels.size() == 1) {
        QSet<SublevelNode*>::const_iterator i = m_selectedSublevels.constBegin();
        (*i)->setSubrange(&subrange);
    }
}
