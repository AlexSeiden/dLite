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
    int yOffset = GuiSettings::paramHeight/2;
    for (ParamBase *param : node->getParams()) {
        y+=GuiSettings::paramHeight;
        ParamItem *parItem = new ParamItem(param, this);
        parItem->setPos(0,y);

        // And build socket items within each param.
        SocketItem *sockItem = new SocketItem(param, parItem);
        parItem->setSocket(sockItem);
        if (param->isOutput())
            sockItem->setPos(GuiSettings::nodeWidth - GuiSettings::socketWidth, yOffset);
        else
            sockItem->setPos(GuiSettings::socketWidth,yOffset);

        // Display an editor widget for the parameter value.
        ParamView *pv = new ParamView(nullptr, param);
        QGraphicsProxyWidget *proxy = this->scene()->addWidget(pv);
#if 0
        // XXX this is broken; for some reason, its setting the value on other connections
        // as well.
        if (param->connectedParam())    //TODO update this when connections happen
            parItem->setEnabled(false);
#endif
        proxy->setParentItem(parItem);
        proxy->setPos(GuiSettings::socketWidth * 2 + 70, 0);
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

//-----------------------------------------------------------------------------
// ParamItem

ParamItem::ParamItem(ParamBase *param, QGraphicsObject *parent) :
    QGraphicsObject(parent),
    _param(param),
    _socket(nullptr)

{ }

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
    _pStart = _serverSocket->scenePos(); // + QPointF(GuiSettings::socketWidth/2., GuiSettings::socketWidth/2.);
    _pEnd   = _clientSocket->scenePos(); // + QPointF(GuiSettings::socketWidth/2., GuiSettings::socketWidth/2.);

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
    painter->setPen(GuiSettings::connectorPen);
    painter->setBrush(Qt::NoBrush);
    if (_path)
        painter->drawPath(*_path);

    // TODO set ellipse pen brush & size
    painter->drawEllipse(0,0,5,5);

    painter->restore();
}
