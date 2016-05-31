#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

#include "engine/utils.h"
#include "views/Transport.h"
#include "views/gui_settings.h"

Transport::Transport(QWidget *parent)
    :   QWidget(parent)
    ,   m_bufferLength(0)
    ,   m_playPosition(0)
    ,   m_segmentation(nullptr)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumSize(300,30);
}

Transport::~Transport() { }

void Transport::reset()
{
    m_bufferLength = 0;
    m_playPosition = 0;
    m_totalDuration = 0;

    update();
}

void Transport::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), guisettings->trans_bgColor);

    if (! m_bufferLength) return;
    painter.save();

    QRect bar = rect();
    const qreal play = qreal(m_playPosition) / m_bufferLength;
    bar.setLeft(rect().left() + play * rect().width());
    bar.setRight(rect().left());
    painter.fillRect(bar, guisettings->trans_barColor);

    if (m_segmentation && m_segmentation->m_segmentIndices.count()) {
        const int segheight = (rect().height()-2)/m_segmentation->m_segmentIndices.count();
        foreach (Segment seg, m_segmentation->m_segments) {
            int segwidth = rect().width()*seg.duration/m_totalDuration;
            int x = float(seg.startTime)*rect().width()/m_totalDuration;
            int y = seg.segmentIndex*segheight+1;
            QRect segrect(x,y, segwidth, segheight);
            painter.fillRect(segrect, guisettings->trans_segmentColor);
            painter.setFont(guisettings->trans_segmentFont);
            painter.setPen(guisettings->trans_segmentTextColor);
            segrect.setBottom(segrect.bottom()+2);
            painter.drawText(segrect, Qt::AlignBottom|Qt::AlignHCenter, seg.segmentVariant);
        }
    }

    // Play head
    QRect playHead(bar.left(), 0, guisettings->trans_playheadWidth, rect().height());
    painter.fillRect(playHead, guisettings->trans_playheadColor);
    painter.restore();
}

void Transport::bufferLengthChanged(qint64 bufferSize)
{
    m_bufferLength = bufferSize;
    m_totalDuration = bytes2ms(bufferSize);
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
    m_segmentation = segmentation;
}

// ------------------------------------------------------------------------------
// Mouse event
//      Move playhead to mouse click point
//      XXX most of the "beat" nodes assume time is going to be increasing
//      without jumps forward or back.
void Transport::mousePressEvent(QMouseEvent *event)
{
    int xpos = event->pos().x();
    float frac = float(xpos)/rect().width();
    emit movePlaybackHead(frac);
}
