#include "NodeItem.h"
#include <QtWidgets>

NodeItem::NodeItem(Node *node, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    dragOver(false),
    color(QColor(120,100,60)),
    _node(node)
{
    setAcceptDrops(true);
    //setZValue((x + y) % 2); // TODO

    setFlags(ItemIsSelectable | ItemIsMovable);
//    setAcceptHoverEvents(true);  do we want these? it was in chip.cpp
}

#if 0
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
#endif

QRectF NodeItem::boundingRect() const
{
    int hh = _node->getParams().size();
    hh++;
    return QRectF(-5, -5, s_width+9, s_height*hh+9);
}

void NodeItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();

    painter->setBrush(dragOver ? color.light(130) : color);
    QBrush b = painter->brush();
    QPen p = painter->pen();

    painter->drawEllipse(-5, -5, 10, 10);

    QRect bigrect(0,0,s_width,s_height*(_node->getParams().size()+1));
    painter->setBrush(Qt::blue);
    painter->drawRect(bigrect);
    painter->setBrush(b);
    if (isSelected()) {
        painter->save();
        painter->setBrush(QBrush());
        QPen selectedPen = QPen(Qt::red);
        selectedPen.setWidth(2);
        painter->setPen(selectedPen);
        QRect biggerRect = bigrect.adjusted(-2,-2,2,2);
        painter->drawRect(biggerRect);
        painter->restore();
    }


    int y = 0;
    // Draw node name
    QRect rr(0,y,s_width,s_height);
    y+=s_height;
    painter->setPen(Qt::white);
    rr.translate(5, 5);
    painter->drawText(rr,_node->getName());
    painter->setPen(p);

    // Draw each param
#if 0
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
#endif
    painter->restore();

}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    update();
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
        // XXX don't really need this stuff....leftover from chip.
        stuff << event->pos();
        update();
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}

ParamItem::ParamItem(ParamBase *param, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _param(param)
{ }

QRectF ParamItem::boundingRect() const
{
    return QRectF(-5, -5, s_width+9, s_height+9);
}

void ParamItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Set box for this param
    QRect rr(0,0,s_width,s_height);

    // Draw rectangle
    painter->setPen(Qt::black);
    painter->drawRoundedRect(rr, 10, 10);

    // Draw name
    painter->setPen(Qt::white);
    rr.translate(25, 5);
    painter->drawText(rr,_param->getName());

    if (_param->isConnectable()) {
        // Draw connector socket
        painter->save();
        painter->setBrush(Qt::red);
        if (_param->isOutput())
            // Draw output socket on right
            painter->drawEllipse(s_width-10, 10, 7, 7);
        else
            // Draw output socket on left
            painter->drawEllipse(10, 10, 7, 7);
        painter->restore();
    }

    painter->restore();
}
