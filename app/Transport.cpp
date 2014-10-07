#include "Transport.h"
#include "GuiSettings.h"
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>
#include <QDebug>
#include "Cupid.h"

Transport::Transport(QWidget *parent)
    :   QWidget(parent)
    ,   m_bufferLength(0)
    ,   m_playPosition(0)
    ,   _segmentation(nullptr)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(30);
    _playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    _pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
}

Transport::~Transport() { }

void Transport::reset()
{
    // XXX when would we really want to call this?
    m_bufferLength = 0;
    m_playPosition = 0;
    _totalDuration = 0;

    update();
}

void Transport::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), GuiSettings::trans_bgColor);

    if (! m_bufferLength) return;


    QRect bar = rect();
    const qreal play = qreal(m_playPosition) / m_bufferLength;
    bar.setLeft(rect().left() + play * rect().width());
    bar.setRight(rect().left());
    painter.fillRect(bar, GuiSettings::trans_barColor);

    if (_segmentation) {
        const int segheight = (rect().height()-2)/_segmentation->_segmentIndices.count();
        foreach (Segment seg, _segmentation->_segments) {
            int segwidth = rect().width()*seg.duration/_totalDuration;
            int x = float(seg.startTime)*rect().width()/_totalDuration;
            int y = seg.segmentIndex*segheight+1;
            QRect segrect(x,y, segwidth, segheight);
            painter.fillRect(segrect, GuiSettings::trans_segmentColor);
            painter.setFont(GuiSettings::trans_segmentFont);
            painter.setPen(GuiSettings::trans_segmentTextColor);
            segrect.setBottom(segrect.bottom()+2);
            painter.drawText(segrect, Qt::AlignBottom|Qt::AlignHCenter, seg.segmentVariant);
        }
    }

    // Play head
    QRect playHead(bar.left(), 0, GuiSettings::trans_playheadWidth, rect().height());
    painter.fillRect(playHead, GuiSettings::trans_playheadColor);

}

void Transport::bufferLengthChanged(qint64 bufferSize)
{
    m_bufferLength = bufferSize;
    _totalDuration = bytes2ms(bufferSize);
    m_playPosition = 0;
    repaint();
}

void Transport::playPositionChanged(qint64 playPosition)
{
    Q_ASSERT(playPosition >= 0);
//    Q_ASSERT(playPosition <= m_bufferLength);
    if  (playPosition > m_bufferLength)
        qDebug() << "ASSERTION FAIL" << Q_FUNC_INFO << playPosition <<m_bufferLength;
    m_playPosition = playPosition;
    repaint();
}

void Transport::segmentsChanged(SongSegmentation *segmentation)
{
    _segmentation = segmentation;
}

// ------------------------------------------------------------------------------
// Mouse event
//      Move playhead to mouse click point
//      XXX most of the "beat" nodes get confused by this.
void Transport::mousePressEvent(QMouseEvent *event)
{
    int xpos = event->pos().x();
    float frac = float(xpos)/rect().width();
    emit movePlaybackHead(frac);
}
