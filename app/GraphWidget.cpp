#include "GraphWidget.h"
#include "NodeItem.h"
#include "Node.h"
#include "CuesheetScene.h"
#include "CuesheetView.h"
#include "GuiSettings.h"
#include "utils.h"
#include <QHBoxLayout>
#include <QGraphicsObject>
#include <QGraphicsProxyWidget>

#include "SublevelNode.h"
#include "SublevelNodeItem.h"
#include "sublevel.h"
#include <QPushButton>

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent),
    _scene(new CuesheetScene),
    _csview(new CuesheetView)
{
    _scene->setSceneRect(QRectF());
    _csview->view()->setScene(_scene);

    CHECKED_CONNECT(_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    // This doesn't work, bc (afaik) the widgets (bc they are gfxitems) don't end up being children of the graphview.
//    setStyleSheet("QLineEdit { background-color: yellow }");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(_csview);
    setGeometry(10, 500, 1500, 500);
    setLayout(layout);
    setWindowTitle("Cuesheet");
    setWindowFlags( Qt::Window | Qt::WindowMaximizeButtonHint |
                    Qt::CustomizeWindowHint);
}


// We need to know when items, like SublevelNode, that have special
// editing are selected, so we can notify other widgets where the
// editing might take place.  For example, SublevelNodes allow dragging
// out a rectangle in the spectrograph, and the spectrograph will display
// the current subrange of a sublevel node.
// Similarly, path & region nodes use the dancefloor widget.
void GraphWidget::selectionChanged()
{
    QList<QGraphicsItem *> selection = _scene->selectedItems();
    QSet<NodeItem*> newSelection;
    foreach (QGraphicsItem *item, selection) {
        NodeItem *nodeItem = dynamic_cast<NodeItem *>(item);
        if (nodeItem)
            newSelection.insert(nodeItem);
    }
    QSet<NodeItem*> itemsDeselected = _wasSelected - newSelection;
    QSet<NodeItem*> itemsNewlySelected = newSelection - _wasSelected;

    foreach (NodeItem* item, itemsDeselected)
        item->beenDeselected();

    foreach (NodeItem* item, itemsNewlySelected)
        item->beenSelected();

    // TODO if no SublevelNode selected, setSubrange = NULL

    _wasSelected = newSelection;
}

void GraphWidget::subrangeHasChanged(Subrange *subrange)
{
    QList<QGraphicsItem *> selection = _scene->selectedItems();
    foreach (QGraphicsItem *item, selection) {
        SublevelNodeItem *nodeItem = dynamic_cast<SublevelNodeItem *>(item);
        int nSublevelNodes = 0;
        if (nodeItem) {
            nSublevelNodes++;
            SublevelNode *node= dynamic_cast<SublevelNode *>(nodeItem->getNode());
            node->setSubrange(subrange);
        }

        if (nSublevelNodes > 1) {
            // ErrorHandling
            qDebug() << "Error: multiple subranges simulatnously selected " << Q_FUNC_INFO << nSublevelNodes;
        }
    }
}

// Adds a list of nodes & their connections to the scene.
// Right now this isn't used; the file read stuff calls
// addNode & addConnection directly so that NodeItem positions
// can be restored.
void GraphWidget::addTheseNodes(QList<Node*> aBunchOfNodes)
{
    // First add all the nodes...
    foreach (Node *node, aBunchOfNodes)
        addNode(node);

    // ...then make all the connections.  Because you can't make the connections before
    // the nodes are instanced!
    foreach (Node *node, aBunchOfNodes) {
        foreach (ParamBase *param, node->getParams()) {
            if (param->connectedParam()) {
                addConnection(param->connectedParam(), param);
            }
        }
    }
}

void GraphWidget::addConnection(ParamBase* server, ParamBase* client)
{
    // Find sockets
   SocketItem* serverSocket = _scene->getSocketForParam(server);
   SocketItem* clientSocket = _scene->getSocketForParam(client);
   if (serverSocket && clientSocket) {
       _scene->connectSockets(serverSocket, clientSocket);
   }
   else {
       // ErrorHandling
       qDebug() << Q_FUNC_INFO << "Could not connect" ;
   }
}

// Style:  This is doing a lot of stuff, that might be better implemented elsewhere?
void GraphWidget::addNode(Node *node, QJsonObject* json)
{
    NodeItem *nodeItem;
    // GROSS
    SublevelNode *sln = dynamic_cast<SublevelNode *>(node);
    if (sln)
        nodeItem = new SublevelNodeItem(node);
    else
        nodeItem = new NodeItem(node);
    nodeItem->setParent(this);

    // TODO better positioning.
    QPointF  center = _csview->view()->mapToScene(_csview->view()->viewport()->rect().center());
    nodeItem->rePos(center);
    _scene->addItem(nodeItem);
    // Moves the node so it isn't right on top of one that's already displayed.
    nodeItem->avoidCollisions();
    _scene->clearSelection();
    nodeItem->setSelected(true);
    _csview->view()->ensureVisible(nodeItem);

    // When restoring from file, read in positon data:
    if (json)
        nodeItem->readFromJSONObj(*json);
}

// ------------------------------------------------------------------------------
// Serialization
void GraphWidget::writeNodeUiToJSONObj(const Node* node, QJsonObject& json)
{
    NodeItem* ni = _scene->getNodeItemForNode(node);
    if (ni)
        ni->writeToJSONObj(json);
    else
        qWarning() << Q_FUNC_INFO << "Could not find NodeItem for " << node->getName();
}

void GraphWidget::readNodeUiFromJSONObj(Node* node, const QJsonObject& json)
{
    NodeItem* ni = _scene->getNodeItemForNode(node);
    // XXX this is called before ni is instatiated....
    if (ni)
        ni->readFromJSONObj(json);
    else
        qWarning() << Q_FUNC_INFO << "Could not find NodeItem for " << node->getName();
}

// ------------------------------------------------------------------------------
// Viewing
void GraphWidget::zoomOut()
{
    _csview->zoomOut(10);
    update();
}

void GraphWidget::zoomIn()
{
    _csview->zoomIn(10);
    update();
}

void GraphWidget::frameItems(QList<QGraphicsItem *> items)
{
    if (items.length() == 0) {
        return;
    }

    QRectF bbox = items[0]->boundingRect();
    foreach (QGraphicsItem *selection, items) {
        bbox = bbox.united(selection->boundingRect());
    }
    if (items.length() == 1)
        _csview->view()->ensureVisible(bbox);
    else
        _csview->fitBbox(bbox);

}

void GraphWidget::frameSelection()
{
    QList<QGraphicsItem *> selection = _scene->selectedItems();
    if (selection.length() == 0) {
        frameAll();
        return;
    }

    frameItems(selection);
}

void GraphWidget::frameAll()
{
    QList<QGraphicsItem *> allItems = _scene->items();
    frameItems(allItems);
}


void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    // frameAll & frameSelected events are handled by a global app shortcut.
    switch (event->key()) {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        deleteSelection();
        update();
        break;
    default:
        qDebug() << Q_FUNC_INFO << "keypress " << event->key() << " ; text()" << event->text();
        event->setAccepted(false);
        QWidget::keyPressEvent(event);
    }
}

void GraphWidget::deleteSelection() {
    QList<QGraphicsItem *> selection = _scene->selectedItems();
    foreach (QGraphicsItem *item, selection) {
        delete item;
    }
}

void GraphWidget::layoutAll()
{
    // TODO lots of hardw
    const int yspacing = 20;

    // Start with Cues, which will be the root of the dag:
    QList<NodeItem*>allCues = _scene->getAllCueNodeItems();
    QPointF nextPos = QPointF(2000., -1000.);
    foreach (NodeItem*ni, allCues) {
        QPointF finalPos = positionNodeItem(ni, nextPos);
        nextPos += QPointF(0, ni->boundingRect().height() + yspacing);
        nextPos.setY(qMax(nextPos.y(), finalPos.y()));
    }
}

QPointF GraphWidget::positionNodeItem(NodeItem* ni, QPointF startPos)
{
    ni->rePos(startPos);
    const int xspacing = 10;
    const int yspacing = 10;
    // Move to the left
    QPointF nextPos = startPos - QPointF(GuiSettings::nodeWidth + xspacing, 0);

    foreach (NodeItem* upstreamItem, ni->getUpstreamNodeItems()) {
        QPointF finalPos = positionNodeItem(upstreamItem, nextPos);
        nextPos += QPointF(0, upstreamItem->boundingRect().height() + yspacing);
        nextPos.setY(qMax(nextPos.y(), finalPos.y()));
    }
    return nextPos;
}
