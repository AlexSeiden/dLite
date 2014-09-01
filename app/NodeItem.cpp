#include "NodeItem.h"
#include "CuesheetScene.h"
#include <QtWidgets>

//-----------------------------------------------------------------------------
// NodeItem

NodeItem::NodeItem(Node *node, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    dragOver(false),
    color(QColor(120,100,60)),
    _node(node)
{
//    setAcceptDrops(true);
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

    painter->setBrush(dragOver ? color.light(130) : color); // XXX handle color differently
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

    painter->restore();

}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Node Item Press Event";
    QGraphicsItem::mousePressEvent(event);
    update();
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Node Item move Event";
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
    qDebug() << "Node Item release Event";
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}

//-----------------------------------------------------------------------------
// ParamItem

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
    painter->setBrush(QColor(120,100,60));
    QRect rr(0,0,s_width,s_height);

    // Draw rectangle
    painter->setPen(Qt::black);
    painter->drawRoundedRect(rr, 10, 10);

    // Draw name
    painter->setPen(Qt::white);
    rr.translate(25, 5);
    painter->drawText(rr,_param->getName());

#if 0
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
#endif

    painter->restore();
}

void ParamItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Param Item Press Event";
    QGraphicsItem::mousePressEvent(event);
    update();
}

void ParamItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Param Item move Event";
    QGraphicsItem::mouseMoveEvent(event);
    update();
}

void ParamItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Param Item Release Event";
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}

//-----------------------------------------------------------------------------
// SocketItem

SocketItem::SocketItem(ParamBase *param, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _param(param)
{ }

QRectF SocketItem::boundingRect() const
{
    return QRectF(-1, -1, s_width+3, s_width+3);
}

void SocketItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw connector socket
    painter->setBrush(Qt::red);
    painter->drawEllipse(0, 0, s_width, s_width);

    painter->restore();
}

void SocketItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Socket Item Press Event";
    qobject_cast<CuesheetScene*>(scene())->startLine(event);
    //QGraphicsItem::mousePressEvent(event);
    update();
}

void SocketItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Socket Item move Event";
    QGraphicsItem::mouseMoveEvent(event);
    update();
}

void SocketItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Socket Item Release Event";
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}


//-----------------------------------------------------------------------------
// ConnectorItem

ConnectorItem::ConnectorItem(ParamBase *param, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _param(param)
{ }

QRectF ConnectorItem::boundingRect() const
{
    return QRectF(-1, -1, 25+3, 12+3);
}

void ConnectorItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw connector socket
    painter->setBrush(Qt::red);
    painter->drawEllipse(0, 0, 25, 10);

    painter->restore();
}

#if 0
void ConnectorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Connector Item Press Event";
    qobject_cast<CuesheetScene*>(scene())->setConnecting(true);
    qobject_cast<CuesheetScene*>(scene())->setStartPoint(event->pos());
    //QGraphicsItem::mousePressEvent(event);
    update();
}

void ConnectorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Connector Item Drag Event";
    QGraphicsItem::mouseMoveEvent(event);
    update();
}

void ConnectorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Connector Item Release Event";
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}
#endif
