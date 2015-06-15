#include "ConnectorItem.h"
#include "NodeItem.h"
#include "CuesheetScene.h"
#include "GuiSettings.h"
#include "utils.h"
#include <QtWidgets>
#include <QPainterPath>
//#include "ParamView.h"

//-----------------------------------------------------------------------------
// ConnectorItem
//      These are used to draw the connections between nodes once they've
//      been connected.

ConnectorItem::ConnectorItem(SocketItem *serverSocket, SocketItem *clientSocket,
                             QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _serverSocket(serverSocket),
    _clientSocket(clientSocket),
    _path(nullptr)
{
    setZValue(1000.0);  // TODO make connector z a pref?
    updatePath();

    setFlags(ItemIsSelectable);

    CHECKED_CONNECT(_serverSocket->parentObject()->parentObject(),
                    SIGNAL(nodeMovedEventSignal()), this, SLOT(gotMoved()));
    CHECKED_CONNECT(_clientSocket->parentObject()->parentObject(),
                    SIGNAL(nodeMovedEventSignal()), this, SLOT(gotMoved()));
}

ConnectorItem::~ConnectorItem()
{
    getClient()->getParam()->disconnect();
    getServer()->getParam()->disconnect();
}

void ConnectorItem::gotMoved()
{
    updatePath();
    update();
}

QRectF ConnectorItem::boundingRect() const
{
    QPointF nStart = _serverSocket->socketPos();
    QPointF nEnd = _clientSocket->socketPos();

    // Compute two rects when dragging.  The first is the rect
    // that connects _pStart & _pEnd -- the PREVIOUS positions.
    // The second rect connects the NEW positions of the sockets--
    // nStart & nEnd. This matters when we are dragging quickly.
    // Hey, do I still have to do this? or was the problem all along the
    // lack of a "prepareGeometryChange()"?


    float leftprev = qMin(_pStart.x(), _pEnd.x()-100);
    float leftnext = qMin( nStart.x(),  nEnd.x()-100);
    float left     = qMin( leftprev,    leftnext);

    float rightprev = qMax(_pStart.x()+100, _pEnd.x());
    float rightnext = qMax( nStart.x()+100,  nEnd.x());
    float right     = qMax( rightprev,       rightnext);

    float topprev = qMin(_pStart.y(), _pEnd.y());
    float topnext = qMin( nStart.y(),  nEnd.y());
    float top     = qMin( topprev,    topnext);

    float bottomprev = qMax(_pStart.y(), _pEnd.y());
    float bottomnext = qMax( nStart.y(),  nEnd.y());
    float bottom     = qMax( bottomprev,  bottomnext);

    QRectF bbox(QPointF(left,top),QPointF(right, bottom));

//    qDebug() << pos() << nStart << nEnd << bbox;
    qreal extra = guisettings->connectorEndSize + 5;
    bbox.adjust(-extra, -extra, extra, extra);
    return bbox;
}

QPainterPath ConnectorItem::shape() const
{
    return(*_path);
}

void ConnectorItem::updatePath()
{
    // Makes a new path when one of the ends has been moved.

    _pStart = _serverSocket->socketPos();
    _pEnd   = _clientSocket->socketPos();

    prepareGeometryChange();
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
    painter->setBrush(Qt::NoBrush);
    if (isSelected())
        painter->setPen(guisettings->connectorPenSelected);
    else
        painter->setPen(guisettings->connectorPen);

    painter->drawPath(*_path);

    // TODO set ellipse pen brush & size
    painter->setBrush(guisettings->connectorBrush);
    painter->drawEllipse(_pStart, guisettings->connectorEndSize, guisettings->connectorEndSize);
    painter->drawEllipse(_pEnd,   guisettings->connectorEndSize, guisettings->connectorEndSize);

    painter->restore();
}


#if 0
#include <math.h>
static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

// Cribbed from "elastic nodes" example
void
drawArrow() {
    // Draw the arrows
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;

    QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
                                              cos(angle - Pi / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
                                              cos(angle - Pi + Pi / 3) * arrowSize);

    painter->setBrush(Qt::black);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}
#endif
