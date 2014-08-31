#include "NodeItem.h"
#include <QtWidgets>

NodeItem::NodeItem(Node *node, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    dragOver(false),
    color(QColor(120,100,60)),
    _node(node)
{
    setAcceptDrops(true);
}

void NodeItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasColor()) {
        event->setAccepted(true);
        dragOver = true;
        update();
    } else {
        event->setAccepted(false);
    }
}

void NodeItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    dragOver = false;
    update();
}

void NodeItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    dragOver = false;
    if (event->mimeData()->hasColor())
        color = qvariant_cast<QColor>(event->mimeData()->colorData());
    update();
}

QRectF NodeItem::boundingRect() const
{
    int hh = _node->getParams().size();
    hh++;
    return QRectF(-5, -5, s_width+5, s_height*hh+5);
}

void NodeItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(dragOver ? color.light(130) : color);
    QBrush b = painter->brush();

    painter->drawEllipse(-5, -5, 10, 10);

    QRect bigrect(0,0,s_width,s_height*(_node->getParams().size()+1));

    painter->setBrush(Qt::blue);
    painter->drawRect(bigrect);
    painter->setBrush(b);
    QPen p = painter->pen();

    int y = 0;
    // Draw node name
    QRect rr(0,y,s_width,s_height);
    y+=s_height;
    painter->setPen(Qt::white);
    rr.translate(5, 5);
    painter->drawText(rr,_node->getName());
    painter->setPen(p);

    // Draw each param
    for (ParamBase *param : _node->getParams()) {
        // Set box for this param
        QRect rr(0,y,s_width,s_height);

        // Draw rectangle
        painter->drawRoundedRect(rr, 10, 10);

        // Draw name
        painter->setPen(Qt::white);
        rr.translate(25, 5);
        painter->drawText(rr,param->getName());
        painter->setPen(p);

        if (param->isConnectable()) {
            // Draw connector
            painter->setBrush(Qt::red);
            if (! param->isOutput())
                painter->drawEllipse(10, y+10, 7, 7);
            else
                painter->drawEllipse(s_width-10, y+10, 7, 7);
            painter->setBrush(b);
        }

        y+=s_height;
    }
}
