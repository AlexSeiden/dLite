#include "CuesheetScene.h"
#include "NodeItem.h"
#include <QGraphicsSceneMouseEvent>
#include "GuiColors.h"
#include <QDebug>


CuesheetScene::CuesheetScene(QObject *parent) :
    QGraphicsScene(parent),
    _line(nullptr),
    _isConnecting(false)
{
}

void CuesheetScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
//    qDebug() << "CuesheetScene Press Event";

    if (_isConnecting) {
//        qDebug() << "CuesheetScene press is connecting";
        startLine(mouseEvent, _sourceSocket);
    }
//    else
//        qDebug() << "CuesheetScene press not connecting";
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void CuesheetScene::startLine(QGraphicsSceneMouseEvent *mouseEvent, SocketItem *srcItem) {
    setConnecting();
    _sourceSocket = srcItem;
    _startPoint = mouseEvent->scenePos();
    _line = new QGraphicsLineItem(QLineF(_startPoint, _startPoint));
    _line->setPen(QPen(GuiColors::connectorColor, 2));
    addItem(_line);
}

void CuesheetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
//    qDebug() << "CuesheetScene move Event";
    if (_isConnecting && _line) {
        // If we are in the middle of making a connection, update the
        // line position
        QLineF newLine(_line->line().p1(), mouseEvent->scenePos());
        _line->setLine(newLine);
        // need "update()" call here??
    } else {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

QGraphicsItem *CuesheetScene::findFirstReleventItem(QList<QGraphicsItem *> &endItems)
{
    QGraphicsItem *out = nullptr;
    foreach (out, endItems) {
        // Don't want to hit the line we're drawing
        if (out == _line)
            continue;

        // Ignore any connectors that we hit
        if (dynamic_cast<ConnectorItem *>(out))
            continue;

        // Can't connect to ourselves
        if (out == dynamic_cast<QGraphicsItem *>(_sourceSocket))
            // Return nullptr rather than continue, because we've actually
            // hit something--ourselves--that's invalid.  We could check to see
            // if there's a *valid* hit behind us, but that's probably more confusing
            // than just making the user drag to what's visible.
            return nullptr;

        SocketItem *targetSocket = getSocket(out);
        // Test for type compatability, source & sink, etc.
        if (! _sourceSocket->getParam()->isConnectableTo(targetSocket->getParam()))
            // Same return logic as above
            return nullptr;


        // OK, nothing wrong with this one.
        return out;
    }

    // Couldn't find any that work
    return nullptr;
}

void CuesheetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "CuesheetScene release Event";
    // XXX nb:  if only one  of "_isConnecting" and "_line" are true, then
    // we have some internal inconsistancy.
    if (_isConnecting && _line) {
        // Find the QGraphicsItems that's underneath the release position.
        QList<QGraphicsItem *> endItems = items(mouseEvent->scenePos());
        QGraphicsItem *targetItem = findFirstReleventItem(endItems);

        // Delete the temporary line we've been dragging out,
        // because soon we'll replace it with the real connector.
        removeItem(_line);
//        delete _line;  XXX this seems to cause crashes.  but is it now leaking mem?
        _line = nullptr;

        if (targetItem) {
            // The target item could be a socket, param, or node.
            SocketItem *targetSocket = getSocket(targetItem);

            if (targetSocket) {
                // Create the actual connection object
                ConnectorItem *connection = new ConnectorItem(_sourceSocket, targetSocket);
                addItem(connection);
                // TODO Tell the sockets about the connection, so they can update them with the position.
                // Or, should sockets just broadcast their positions via signals, and connections listen via slots?
    //            _sourceSocket->addConnection(connection);
    //            targetSocket->addConnection(connection);
            }
        }
    }

    _line = nullptr;
    setConnecting(false);
    setStartPoint(QPointF());
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

SocketItem *CuesheetScene::getSocket(QGraphicsItem *item)
{
    // Did we drag directly to a socket?
    SocketItem *sock;
    sock = dynamic_cast<SocketItem *>(item);
    if (sock)
        return sock;

    // OK, that didn't work.  Let's try a param.
    ParamItem *paramItem;
    paramItem = dynamic_cast<ParamItem *>(item);
    if (paramItem) {
        return paramItem->getSocket();
    }

#if 0
    // That didn't work either.  Pehaps it's a node--look for it's output connection:
    NodeItem *node;
    node = dynamic_cast<NodeItem *>(item);
    if (node) {
        return node->getOutputSocket();
    }
#endif

    // Fuck it, must be something we can't connect to anyway.
    qDebug() << "can't connect to type: " << typeid(item).name() << " of item " << item;
    return nullptr;
}
