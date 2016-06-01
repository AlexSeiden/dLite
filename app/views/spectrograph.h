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
