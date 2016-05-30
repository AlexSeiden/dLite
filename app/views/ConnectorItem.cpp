#include "views/ConnectorItem.h"
#include "views/NodeItem.h"
#include "views/CuesheetScene.h"
#include "views/GuiSettings.h"
#include "engine/utils.h"
#include <QtWidgets>
#include <QPainterPath>

static const int offset = 25;

ConnectorStub::ConnectorStub(QGraphicsItem *parent, bool isOutput) :
    QGraphicsObject(parent),
    m_isOutput(isOutput),
    m_path(nullptr)
{
    setZValue(1000.0);
    updatePath();
}

ConnectorStub::~ConnectorStub() { }

void ConnectorStub::updatePath()
{
    // Makes a new path when one of the ends has been moved.
    QPointF p_end;

    prepareGeometryChange();
    if (m_path)
        delete m_path;

    m_path = new QPainterPath();
    if (m_isOutput) {
        p_end = m_p + QPointF(offset,0);
    } else {
        p_end = m_p - QPointF(offset,0);
    }

    m_path->moveTo(m_p);
    m_path->lineTo(p_end);
}

void ConnectorStub::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(guisettings->m_connectorPen);
    painter->drawPath(*m_path);

    painter->restore();
}

void ConnectorStub::gotMoved()
{
    updatePath();
    update();
}

QRectF ConnectorStub::boundingRect() const
{
    float left, right, top, bottom;
    top = m_p.y() - 2;
    bottom = m_p.y() + 2;
    if (m_isOutput) {
        left = m_p.x();
        right = left + offset;
    }
    else {
        right = m_p.x();
        left = right - offset;
    }
    QRectF bbox(QPointF(left,top),QPointF(right, bottom));
    return bbox;
}

//-----------------------------------------------------------------------------
// ConnectorItem
//      These are used to draw the connections between nodes once they've
//      been connected.

ConnectorItem::ConnectorItem(SocketItem *serverSocket, SocketItem *clientSocket,
                             QGraphicsItem *parent) :
    QGraphicsObject(parent),
    m_serverSocket(serverSocket),
    m_clientSocket(clientSocket),
    m_path(nullptr),
    m_startStub(this, true),
    m_endStub(this, false)
{
    setZValue(-1000.0);  // TODO make connector z a pref?
    updatePath();

    setFlags(ItemIsSelectable);

    CHECKED_CONNECT(m_serverSocket->parentObject()->parentObject(),
                    SIGNAL(nodeMovedEventSignal()), this, SLOT(gotMoved()));
    CHECKED_CONNECT(m_clientSocket->parentObject()->parentObject(),
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
    QPointF nStart = m_serverSocket->socketPos();
    QPointF nEnd = m_clientSocket->socketPos();

    // Compute two rects when dragging.  The first is the rect
    // that connects _pStart & _pEnd -- the PREVIOUS positions.
    // The second rect connects the NEW positions of the sockets--
    // nStart & nEnd. This matters when we are dragging quickly.
    // TODO Can this be merged into "prepareGeometryChange()"?

    float leftprev = qMin(m_pStart.x(), m_pEnd.x()-100);
    float leftnext = qMin( nStart.x(),  nEnd.x()-100);
    float left     = qMin( leftprev,    leftnext);

    float rightprev = qMax(m_pStart.x()+100, m_pEnd.x());
    float rightnext = qMax( nStart.x()+100,  nEnd.x());
    float right     = qMax( rightprev,       rightnext);

    float topprev = qMin(m_pStart.y(), m_pEnd.y());
    float topnext = qMin( nStart.y(),  nEnd.y());
    float top     = qMin( topprev,    topnext);

    float bottomprev = qMax(m_pStart.y(), m_pEnd.y());
    float bottomnext = qMax( nStart.y(),  nEnd.y());
    float bottom     = qMax( bottomprev,  bottomnext);

    QRectF bbox(QPointF(left,top),QPointF(right, bottom));

    qreal extra = guisettings->m_connectorEndSize + 5;
    bbox.adjust(-extra, -extra, extra, extra);
    return bbox;
}

QPainterPath ConnectorItem::shape() const
{
    return(*m_path);
}


void ConnectorItem::updatePath()
{
    // Makes a new path when one of the ends has been moved.

    m_pStart = m_serverSocket->socketPos();
    m_startStub.m_p = m_pStart;
    m_pEnd   = m_clientSocket->socketPos();
    m_endStub.m_p = m_pEnd;

    m_pStart += QPointF(offset,0);
    m_pEnd -= QPointF(offset,0);

    qreal deltaX = fabs(m_pEnd.x() - m_pStart.x());
    qreal tangent = .5 * deltaX;

    prepareGeometryChange();
    // TODO do we always have to delete the path and allocate again?
    // Can't we have a member path?
    if (m_path)
        delete m_path;

    m_path = new QPainterPath();
    m_path->moveTo(m_pStart);
    m_path->cubicTo(m_pStart+QPointF(tangent,0.), m_pEnd+QPointF(-tangent,0.), m_pEnd);

    m_startStub.gotMoved();
    m_endStub.gotMoved();

}

void ConnectorItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setBrush(Qt::NoBrush);
    if (isSelected())
        painter->setPen(guisettings->m_connectorPenSelected);
    else
        painter->setPen(guisettings->m_connectorPen);

    painter->drawPath(*m_path);
    painter->restore();
}


// TODO
#if 0
#include <math.h>
static const double Pi = 3.141592653589793238;
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
