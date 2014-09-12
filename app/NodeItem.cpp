#include "NodeItem.h"
#include "CuesheetScene.h"
#include "GuiColors.h"
#include "utils.h"
#include <QtWidgets>
#include <QPainterPath>
#include "ParamView.h"
#include <QGraphicsProxyWidget>

// Classes for drawing the graph view representation of all the nodes.

//-----------------------------------------------------------------------------
// NodeItem

NodeItem::NodeItem(Node *node, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    dragOver(false),
    _node(node)
{
//    setAcceptDrops(true);
    //setZValue((x + y) % 2); // TODO

    setFlags(ItemIsSelectable | ItemIsMovable);

    // Make a ParamItem for every param in the node:
    int y = 0;
    for (ParamBase *param : node->getParams()) {
        y+=GuiSettings::paramHeight;
        ParamItem *parItem = new ParamItem(param, this);
        parItem->setPos(0,y);

    }
}

QRectF NodeItem::boundingRect() const
{
    int hh = _node->getParams().size() + 1;  // + 1 for the node name
    return QRectF(-5, -5, GuiSettings::nodeWidth+9, GuiSettings::paramHeight*hh+9);
}

void NodeItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
// XXX I don't think I'm handling the QStyleOptions stuff correctly.
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();

    //painter->setBrush(dragOver ? color.light(130) : GuiColors::nodeBGColor);
    // TODO drag-over highlighting
    painter->setBrush(GuiSettings::nodeBGColor); // TODO color by type

    QBrush b = painter->brush();
    QPen p = painter->pen();

    QRect bigrect(0,0,GuiSettings::nodeWidth,GuiSettings::paramHeight*(_node->getParams().size()+1));
    painter->setBrush(GuiSettings::nodeNameColor);
    painter->drawRect(bigrect);
    painter->setBrush(b);
    if (isSelected()) {
        painter->save();
        painter->setBrush(QBrush());
        QPen selectedPen = QPen(GuiSettings::selectedNodePenColor);
        selectedPen.setWidth(GuiSettings::selectedNodePenWidth);
        painter->setPen(selectedPen);
        QRect biggerRect = bigrect.adjusted(-2,-2,2,2);
        painter->drawRect(biggerRect);
        painter->restore();
    }


    int y = 0;
    // Draw node name
    QRect rr(0,y,GuiSettings::nodeWidth,GuiSettings::paramHeight);
    y+=GuiSettings::paramHeight;
    painter->setPen(GuiSettings::nodeTextColor);
    rr.translate(5, 5);
    painter->drawText(rr,_node->getName());
    painter->setPen(p);

    painter->restore();

}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Used for updating any attached connectors.
    emit nodeMovedEventSignal();
    QGraphicsItem::mouseMoveEvent(event);
}

void NodeItem::keyPressEvent(QKeyEvent *event)
{
    // XXX  this isn't working
    qDebug() << "keypress " << event->key();
    switch (event->key()) {
    case Qt::Key_Delete:
        // XXX this will probably break. need to make sure dtors do right thing.
        delete this->_node;
        this->scene()->removeItem(this);
        deleteLater();
        update();
        break;
    default:
        QGraphicsItem::keyPressEvent(event);
    }
}

void NodeItem::beenSelected()
{
    // Forward selection to node, in case there's anything special to do
    // (e.g. Sublevel nodes need to interact with the spectrograph;
    //  path nodes will interact with the floor display.)
    _node->beenSelected();
}

void NodeItem::avoidCollisions()
{
   QList<QGraphicsItem *>colliders = collidingItems(Qt::IntersectsItemBoundingRect);
//   QList<QGraphicsItem *>colliders = collidingItems();

   foreach (QGraphicsItem *item, colliders) {
       NodeItem *ni = dynamic_cast<NodeItem *>(item);
       if (ni) {
           moveBy(GuiSettings::nodeWidth + GuiSettings::nodeSpacing, 0);
           avoidCollisions();
           break;
       }
   }
}

//-----------------------------------------------------------------------------
// ParamItem

ParamItem::ParamItem(ParamBase *param, QGraphicsObject *parent) :
    QGraphicsObject(parent),
    _param(param)
{
    int yOffset = GuiSettings::paramHeight/2;

    // Build socket items
    _socket = new SocketItem(param, this);
    if (param->isOutput())
        // Output items get their socket on the right side
        _socket->setPos(GuiSettings::nodeWidth - GuiSettings::socketWidth, yOffset);
    else {
        // Input items get their socket on the left side, and get an editor
        // proxyWidget too.
        _socket->setPos(GuiSettings::socketWidth,yOffset);

        // Display an editor widget for the parameter value.
        ParamView *pv = new ParamView(nullptr, param);
        QGraphicsProxyWidget *proxy = this->scene()->addWidget(pv);
#if 0
        // XXX this is broken; for some reason, it's setting the value on other connections
        // as well.
        if (param->connectedParam())    //TODO update this when connections happen
            parItem->setEnabled(false);
#endif
        proxy->setParentItem(this);
        proxy->setPos(GuiSettings::socketWidth * 2 + 70, 0);
    }
}


QRectF ParamItem::boundingRect() const
{
    return QRectF(-5, -5, GuiSettings::nodeWidth+9, GuiSettings::paramHeight+9);
}

void ParamItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Set box for this param
    if (_param->isOutput())
        painter->setBrush(GuiSettings::outputParamFillColor);
    else
        painter->setBrush(GuiSettings::paramFillColor);
    QRect rr(0,0,GuiSettings::nodeWidth,GuiSettings::paramHeight);

    // Draw rectangle
    painter->setPen(Qt::black);
    painter->drawRoundedRect(rr, 10, 10);

    // Draw name
    painter->setPen(GuiSettings::paramTextColor);
    rr.translate(GuiSettings::socketWidth*2 + 20, 5);
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
    QRectF out = QRectF(-GuiSettings::socketWidth/2, -GuiSettings::socketWidth/2,
                         GuiSettings::socketWidth,  GuiSettings::socketWidth);
    out.adjust(-2,-2,2,2);
    return out;
}

void SocketItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw connector socket
    painter->setBrush(GuiSettings::socketFillColor);
    painter->setPen(GuiSettings::socketOutlinePen);
//    painter->drawEllipse(0, 0, GuiSettings::socketWidth, GuiSettings::socketWidth);
    painter->drawRect(-GuiSettings::socketWidth/2, -GuiSettings::socketWidth/2,
                      GuiSettings::socketWidth, GuiSettings::socketWidth);

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
    // XXX doesn't correctly handle the cubic.

    qreal extra = 20.; // XXX kinda arbitrary
    QPointF nStart = _serverSocket->scenePos();
    QPointF nEnd = _clientSocket->scenePos();

    // Compute two rects when dragging.  The first is the rect
    // that connects _pStart & _pEnd -- the PREVIOUS positions.
    QRectF bbox(_pStart, QSizeF(qMax(_pEnd.x() - _pStart.x(),100.), _pEnd.y() - _pStart.y()));
    bbox = bbox.normalized();

    // The second rect connects the NEW positions of the sockets.
    // This matters when we are dragging quickly.
    QRectF nbox(_pStart, QSizeF(qMax(nEnd.x() - nStart.x(),100.), nEnd.y() - nStart.y()));
    nbox = nbox.normalized();

    // Take the union of both:
    bbox = bbox.united(nbox);

    // And add a fudge factor
    bbox = bbox.adjusted(-extra, -extra, extra, extra);
    return bbox;
}

QPainterPath ConnectorItem::shape() const
{
    return(*_path);
}

void ConnectorItem::updatePath()
{
    _pStart = _serverSocket->scenePos();
    _pEnd   = _clientSocket->scenePos();

    if (_path)
        delete _path;

    _path = new QPainterPath();
    _path->moveTo(_pStart);
    _path->cubicTo(_pStart+QPointF(100.,0.), _pEnd+QPointF(-100.,0.), _pEnd);
}

void ConnectorItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setPen(GuiSettings::connectorPen);
    painter->setBrush(Qt::NoBrush);
    if (_path)
        painter->drawPath(*_path);

    // TODO set ellipse pen brush & size
    painter->drawEllipse(_pStart, GuiSettings::connectorEndSize, GuiSettings::connectorEndSize);
    painter->drawEllipse(_pEnd,   GuiSettings::connectorEndSize, GuiSettings::connectorEndSize);

    painter->restore();
}
