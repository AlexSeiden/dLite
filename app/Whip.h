#ifndef WHIP_H
#define WHIP_H

#include <QPushButton>
#include <QRubberBand>
#include <QMouseEvent>

class Whip : public QPushButton
{
    Q_OBJECT
public:
    explicit Whip(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:

private:
    bool   				m_isDragging;
    QPoint				m_dragStart;
    QRubberBand*		m_rubberBand;
};

#endif // WHIP_H
