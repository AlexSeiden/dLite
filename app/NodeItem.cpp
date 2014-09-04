#include "NodeItem.h"
#include "CuesheetScene.h"
#include "GuiColors.h"
#include "utils.h"
#include <QtWidgets>
#include <QPainterPath>

// Classes for drawing the graph view representation of all the nodes.

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

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit nodeMovedEventSignal();
    QGraphicsItem::mouseMoveEvent(event);
}

//-----------------------------------------------------------------------------
// ParamItem

ParamItem::ParamItem(ParamBase *param, QGraphicsObject *parent) :
    QGraphicsObject(parent),
    _param(param),
    _socket(nullptr)

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
    painter->restore();
}

//-----------------------------------------------------------------------------
// SocketItem

SocketItem::SocketItem(ParamBase *param, QGraphicsObject *parent) :
    QGraphicsObject(parent),
    _param(param)
{ }

QRectF SocketItem::boundingRect() const {
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
    qobject_cast<CuesheetScene*>(scene())->startLine(event, this);
    update();
}


//-----------------------------------------------------------------------------
// ConnectorItem

ConnectorItem::ConnectorItem(SocketItem *serverSocket, SocketItem *clientSocket,
                             QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _serverSocket(serverSocket),
    _clientSocket(clientSocket),
    _path(nullptr)
{
    // Connection arrows are always in back
    setZValue(1000.0);
// TODO     setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    updatePath();

    CHECKED_CONNECT(_serverSocket->parentObject()->parentObject(),
                    SIGNAL(nodeMovedEventSignal()), this, SLOT(gotMoved()));
    CHECKED_CONNECT(_clientSocket->parentObject()->parentObject(),
                    SIGNAL(nodeMovedEventSignal()), this, SLOT(gotMoved()));

}

void ConnectorItem::gotMoved()
{
    updatePath();
    update();
}

QRectF ConnectorItem::boundingRect() const
{
    qreal extra = 20.; // XXX kinda arbitrary
    QPointF nStart = _serverSocket->scenePos();
    QPointF nEnd = _clientSocket->scenePos();

    QRectF bbox(_pStart, QSizeF(_pEnd.x() - _pStart.x(), _pEnd.y() - _pStart.y()));
    bbox = bbox.normalized();

    QRectF nbox(_pStart, QSizeF(nEnd.x() - nStart.x(), nEnd.y() - nStart.y()));
    nbox = nbox.normalized();

    bbox = bbox.united(nbox);

    bbox = bbox.adjusted(-extra, -extra, extra, extra);
    return bbox;
}

void ConnectorItem::updatePath()
{
    _pStart = _serverSocket->scenePos() + QPointF(SocketItem::s_width/2., SocketItem::s_width/2.);
    _pEnd   = _clientSocket->scenePos() + QPointF(SocketItem::s_width/2., SocketItem::s_width/2.);

    if (_path)
        delete _path;

    _path = new QPainterPath();
    _path->moveTo(_pStart);
    _path->cubicTo(_pStart+QPointF(100,0), _pEnd+QPointF(-100,0), _pEnd);
}

void ConnectorItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    QPen pen(GuiColors::connectorColor);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    if (_path)
        painter->drawPath(*_path);
    painter->restore();
}
