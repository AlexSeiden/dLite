#include "progressbar.h"
#include "spectrum.h"
#include <QPainter>

ProgressBar::ProgressBar(QWidget *parent)
    :   QWidget(parent)
    ,   m_bufferLength(0)
    ,   m_recordPosition(0)
    ,   m_playPosition(0)
    ,   m_windowPosition(0)
    ,   m_windowLength(0)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(30);
}

ProgressBar::~ProgressBar() { }

void ProgressBar::reset()
{
    m_bufferLength = 0;
    m_recordPosition = 0;
    m_playPosition = 0;
    m_windowPosition = 0;
    m_windowLength = 0;
    update();
}

void ProgressBar::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    QColor bufferColor(0, 0, 255);
    QColor windowColor(0, 255, 0);

    painter.fillRect(rect(), Qt::black);

    if (m_bufferLength) {
        QRect bar = rect();
        const qreal play = qreal(m_playPosition) / m_bufferLength;
        bar.setLeft(rect().left() + play * rect().width());
        const qreal record = qreal(m_recordPosition) / m_bufferLength;
        bar.setRight(rect().left() + record * rect().width());
        painter.fillRect(bar, bufferColor);

        QRect window = rect();
        const qreal windowLeft = qreal(m_windowPosition) / m_bufferLength;
        window.setLeft(rect().left() + windowLeft * rect().width());
        const qreal windowWidth = qreal(m_windowLength) / m_bufferLength;
        window.setWidth(windowWidth * rect().width());
        painter.fillRect(window, windowColor);
    }
}

void ProgressBar::bufferLengthChanged(qint64 bufferSize)
{
    m_bufferLength = bufferSize;
    m_recordPosition = 0;
    m_playPosition = 0;
    m_windowPosition = 0;
    m_windowLength = 0;
    repaint();
}

void ProgressBar::recordPositionChanged(qint64 recordPosition)
{
    Q_ASSERT(recordPosition >= 0);
    Q_ASSERT(recordPosition <= m_bufferLength);
    m_recordPosition = recordPosition;
    repaint();
}

void ProgressBar::playPositionChanged(qint64 playPosition)
{
    Q_ASSERT(playPosition >= 0);
    Q_ASSERT(playPosition <= m_bufferLength);
    m_playPosition = playPosition;
    repaint();
}

void ProgressBar::windowChanged(qint64 position, qint64 length)
{
    Q_ASSERT(position >= 0);
    Q_ASSERT(position <= m_bufferLength);
    Q_ASSERT(position + length <= m_bufferLength);
    m_windowPosition = position;
    m_windowLength = length;
    repaint();
}
