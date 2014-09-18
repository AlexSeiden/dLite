#include "Transport.h"
#include "GuiSettings.h"
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>
#include <QDebug>

Transport::Transport(QWidget *parent)
    :   QWidget(parent)
    ,   m_bufferLength(0)
    ,   m_playPosition(0)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(30);
    _playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    _pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
}

Transport::~Transport() { }

void Transport::reset()
{
    m_bufferLength = 0;
    m_playPosition = 0;
    update();
}

void Transport::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), GuiSettings::trans_bgColor);

    if (m_bufferLength) {
        QRect bar = rect();
        const qreal play = qreal(m_playPosition) / m_bufferLength;
        bar.setLeft(rect().left() + play * rect().width());
        bar.setRight(rect().left());
        painter.fillRect(bar, GuiSettings::trans_barColor);
    }
}

void Transport::bufferLengthChanged(qint64 bufferSize)
{
    m_bufferLength = bufferSize;
    m_playPosition = 0;
    repaint();
}

void Transport::playPositionChanged(qint64 playPosition)
{
    Q_ASSERT(playPosition >= 0);
    Q_ASSERT(playPosition <= m_bufferLength);
    m_playPosition = playPosition;
    repaint();
}


void Transport::mousePressEvent(QMouseEvent *event)
{
    int xpos = event->pos().x();
    float frac = float(xpos)/rect().width();

    qDebug() << "Press:  " << frac;
}

void Transport::mouseMoveEvent(QMouseEvent *event)
{
}

void Transport::mouseReleaseEvent(QMouseEvent *event)
{
}
