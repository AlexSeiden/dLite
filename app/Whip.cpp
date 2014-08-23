#include "Whip.h"
#include <QDebug>

Whip::Whip(QWidget *parent) :
    QPushButton(parent)
{
    m_rubberBand = nullptr;
}

void
Whip::mousePressEvent(QMouseEvent *event)
{
    m_dragStart = event->pos();
    qDebug() << m_dragStart;
    if (!m_rubberBand)
        m_rubberBand = new QRubberBand(QRubberBand::Line, NULL);
    m_rubberBand->setGeometry(QRect(m_dragStart, QSize()));
    m_rubberBand->show();
}

void Whip::mouseMoveEvent(QMouseEvent *event)
{
    //m_rubberBand->setGeometry(QRect(m_dragStart, event->pos()).normalized());
}

void Whip::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    QRect geo = m_rubberBand->geometry();
    geo = geo.intersected(rect());

    // Redraw
    m_rubberBand->hide();
    this->update();
}
