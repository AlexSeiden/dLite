#ifndef NODEITEM_H
#define NODEITEM_H

#include "Param.h"
#include "Node.h"
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QList>


QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class NodeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NodeItem(Node *node, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:

public slots:

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

    bool dragOver;
    QColor  color;

private:
    Node    *_node;
    static const int s_width=100;
    static const int s_height=25;
};

#endif // NODEITEM_H
