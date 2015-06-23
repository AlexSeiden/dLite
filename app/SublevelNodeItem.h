#ifndef SUBLEVELNODEITEM_H
#define SUBLEVELNODEITEM_H
#include "NodeItem.h"
#include <QWidget>
#include <QPainter>

class SublevelNode;

// A horizontal visual display of the level measured by a SublevelNode.
class RangeMeter : public QWidget
{
    Q_OBJECT

public:
    explicit RangeMeter(QWidget *parent = 0);
    ~RangeMeter();

    void    paintEvent(QPaintEvent *event);

public slots:
    void    levelChanged(qreal level);

private:
    // Height of level bar.
    float   m_level;
};


class SublevelNodeItem : public NodeItem
{
public:
    explicit    SublevelNodeItem(Node *node, QGraphicsItem *parent = 0);

    QRectF      boundingRect() const;
    void        minimize(int status);

private:
    RangeMeter*     m_rangeMeter;
    SublevelNode*   m_sublevelNode;
};

#endif // SUBLEVELNODEITEM_H
