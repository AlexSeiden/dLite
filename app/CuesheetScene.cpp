#include "CuesheetScene.h"
#include "NodeItem.h"
#include "ConnectorItem.h"
#include "GuiSettings.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "GroupNodeItem.h"


CuesheetScene::CuesheetScene(QObject *parent) :
    QGraphicsScene(parent),
    _line(nullptr),
    _isConnecting(false)
{
}

// ------------------------------------------------------------------------------
// Overrides
void CuesheetScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (_isConnecting) {
        startLine(mouseEvent, _startSocket);
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void CuesheetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
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

void CuesheetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Finish a possible connection action:  verify if we have a valid connection,
    // and then wire things together if appropriate.  Either way, clean up.

    // ErrorHandling:  If only one of "_isConnecting" and "_line" are true, then
    // we have some internal inconsistancy.
    if (_isConnecting && _line) {
        // Find the QGraphicsItems that's underneath the release position.
        QList<QGraphicsItem *> endItems = items(mouseEvent->scenePos());
        QGraphicsItem *targetItem = findFirstReleventItem(endItems);

        // Delete the temporary line we've been dragging out,
        // because soon we'll replace it with the real connector.
        removeItem(_line);
//        delete _line;  XXX this seems to cause crashes--but is it now leaking mem?
        _line = nullptr;

        if (targetItem) {
            // The target item could be a socket, param, or node.
            SocketItem *targetSocket = getSocket(targetItem);

            if (targetSocket) {
                // Create the actual connection object
                SocketItem *server, *client;
                // This assumes that _startSocket and targetSocket have already
                // been verified as being compatable:  i.e., one's an input, the
                // other an output; they have the same type; etc.  This is done
                // when findFirstReleventItem() calls isConnectableTo()
                if (_startSocket->getParam()->isOutput()) {
                    server = _startSocket;
                    client = targetSocket;
                } else {
                    server = targetSocket;
                    client = _startSocket;
                }

                // If this client already has a connection, we need to delete it's
                // visual representation:
                if (client->getParam()->connectedParam()) {
                    ConnectorItem* cnctr = getConnectorForClient(client->getParam());
                    removeItem(cnctr);
                    delete cnctr;
                }

                connectSockets(server, client);
                // Make the actual connection between parameters.
                // GROSS -- sholdn't this be handled in the model?
                client->getParam()->connectTo(server->getParam());
            }
        }
    }

    _line = nullptr;
    setConnecting(false);
    setStartPoint(QPointF());
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
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
        if (out == dynamic_cast<QGraphicsItem *>(_startSocket))
            // Return nullptr rather than continue, because we've actually
            // hit something--ourselves--that's invalid.  We could check to see
            // if there's a *valid* hit behind us, but that's probably more confusing
            // than just making the user drag to what's visible.
            return nullptr;

        SocketItem *targetSocket = getSocket(out);
        // Test for type compatability, source & sink, etc.
        if (! targetSocket)
            return nullptr;

        if (! _startSocket->getParam()->isConnectableTo(targetSocket->getParam()))
            // Same return logic as above
            return nullptr;


        // OK, nothing wrong with this one.
        return out;
    }

    // Couldn't find any that work
    return nullptr;
}

void CuesheetScene::startLine(QGraphicsSceneMouseEvent *mouseEvent, SocketItem *srcItem) {
    setConnecting();
    _startSocket = srcItem;
    _startPoint = mouseEvent->scenePos();
    _line = new QGraphicsLineItem(QLineF(_startPoint, _startPoint));
    _line->setPen(QPen(GuiSettings::connectorColor, 2));
    addItem(_line);
}

// ------------------------------------------------------------------------------
// Query routines
void CuesheetScene::connectSockets(SocketItem *server, SocketItem *client)
{
    ConnectorItem *connection = new ConnectorItem(server, client);
    addItem(connection);
}

SocketItem *CuesheetScene::getSocket(QGraphicsItem *item)
{
    // Did we drag directly to a socket?
    SocketItem *sock;
    sock = dynamic_cast<SocketItem *>(item);
    if (sock)
        return sock;

    // OK, it's not a socket.  Let's try a param.
    ParamItem *paramItem;
    paramItem = dynamic_cast<ParamItem *>(item);
    if (paramItem) {
        return paramItem->getSocket();
    }

#if 0
    // TODO That didn't work either.  Pehaps it's a node--look for it's output connection:
    NodeItem *node;
    node = dynamic_cast<NodeItem *>(item);
    if (node) {
        return node->getOutputSocket();
    }
#endif

    // Fuck it, must be something we can't connect to anyway.
    qWarning() <<Q_FUNC_INFO<< "can't connect to type: " << typeid(item).name() << " of item " << item;
    return nullptr;
}

// The following are super inefficient from an algorithmic point of view,
// but in practical terms it shouldn't be a problem.  If it is, we'll
// optimize then.
SocketItem *CuesheetScene::getSocketForParam(const ParamBase *param)
{
    QList<QGraphicsItem*>allItems = items();
    foreach (QGraphicsItem* item, allItems) {
        SocketItem *sock = dynamic_cast<SocketItem *>(item);
        if (! sock)
            continue;
        if (sock->getParam() == param)
            return sock;
    }
    return nullptr;
}

ConnectorItem *CuesheetScene::getConnectorForClient(const ParamBase *client)
{
    // Returns a connector that is connected to the given input (client) param.
    QList<QGraphicsItem*>allItems = items();
    foreach (QGraphicsItem* item, allItems) {
        ConnectorItem *cnctr = dynamic_cast<ConnectorItem *>(item);
        if (! cnctr)
            continue;
        if (cnctr->getClient()->getParam() == client)
            return cnctr;
    }
    return nullptr;
}

ConnectorItem *CuesheetScene::getConnectorForParam(const ParamBase *param)
{
    // Returns a connector that is connected to the given param either as
    // a client or a server.
    QList<QGraphicsItem*>allItems = items();
    foreach (QGraphicsItem* item, allItems) {
        ConnectorItem *cnctr = dynamic_cast<ConnectorItem *>(item);
        if (! cnctr)
            continue;
        if (cnctr->getClient()->getParam() == param)
            return cnctr;
        if (cnctr->getServer()->getParam() == param)
            return cnctr;
    }
    return nullptr;
}

// Returns the NodeItem that represents a given Node.
// Again, O(n) in number of nodes; could use hash map, could attach NodeItem ptr to
// Node, probably doesn't matter.
NodeItem* CuesheetScene::getNodeItemForNode(const Node* node)
{
    QList<QGraphicsItem*>allItems = items();
    foreach (QGraphicsItem* item, allItems) {
        NodeItem *ni = dynamic_cast<NodeItem *>(item);
        if (! ni)
            continue;
        if (ni->getNode() == node)
            return ni;
    }
    return nullptr;
}

void CuesheetScene::selectTheseNodes(QList<Node*> selection)
{
    clearSelection();
    foreach (Node* node, selection) {
        NodeItem *ni = getNodeItemForNode(node);
        ni->setSelected(true);
    }
}

QList<NodeItem*> CuesheetScene::getAllCueNodeItems()
{
    QList<QGraphicsItem*>allItems = items();
    QList<NodeItem*>allCues;
    foreach (QGraphicsItem* item, allItems) {
        NodeItem *ni = dynamic_cast<NodeItem *>(item);
        if (! ni)
            continue;
        if (ni->getNode()->getType() == Node::CUE)
            allCues << ni;
    }
    return allCues;
}

QList<NodeItem*> CuesheetScene::getAllNodeItems() const
{
    QList<QGraphicsItem*>allItems = items();
    QList<NodeItem*>allNodes;
    foreach (QGraphicsItem* item, allItems) {
        NodeItem *ni = dynamic_cast<NodeItem *>(item);
        if (! ni)
            continue;
        allNodes << ni;
    }
    return allNodes;
}

QList<NodeItem*> CuesheetScene::getSelectedNodeItems()
{
    QList<QGraphicsItem*>allItems = selectedItems();
    QList<NodeItem*>allNodes;
    foreach (QGraphicsItem* item, allItems) {
        NodeItem *ni = dynamic_cast<NodeItem *>(item);
        if (! ni)
            continue;
        allNodes << ni;
    }
    return allNodes;
}

QRectF CuesheetScene::getItemsBBox(QList<QGraphicsItem*> items) const
{
    QRectF bbox;
    foreach (QGraphicsItem* item, items) {
        bbox = bbox.united(item->boundingRect());
    }
    return bbox;
}

QList<QGraphicsItem*> CuesheetScene::getSelectedGroupableItems()
{
    QList<QGraphicsItem*>allItems = selectedItems();
    QList<QGraphicsItem*>groupableItems;
    foreach (QGraphicsItem* item, allItems) {
        NodeItem *ni = dynamic_cast<NodeItem *>(item);
        if (ni) {
            groupableItems << ni;
            continue;
        }
        GroupNodeItem *gni = dynamic_cast<GroupNodeItem *>(item);
        if (gni) {
            groupableItems << gni;
            continue;
        }
    }
    return groupableItems;
}

QList<GroupNodeItem*> CuesheetScene::getSelectedGroups()
{
    QList<QGraphicsItem*>allItems = selectedItems();
    QList<GroupNodeItem*>groupItems;
    foreach (QGraphicsItem* item, allItems) {
        GroupNodeItem *gni = dynamic_cast<GroupNodeItem *>(item);
        if (gni) {
            groupItems << gni;
            continue;
        }
    }
    return groupItems;
}

QList<Node*> CuesheetScene::getNodes() const
{
    QList<NodeItem *> selection = getAllNodeItems();
    QList<Node*> out;
    foreach (NodeItem* item, selection) {
        out << item->getNode();
    }
    return out;
}
