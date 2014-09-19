#include "GraphWidget.h"
#include "NodeItem.h"
#include "Node.h"
#include "CuesheetScene.h"
#include "GuiSettings.h"
#include "utils.h"
#include "ParamView.h"
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
    _scene->setSceneRect(-3000.,-3000.,6000.,6000.);
    _csview->view()->setScene(_scene);

    CHECKED_CONNECT(_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

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

void GraphWidget::selectionChanged()
{
    QList<QGraphicsItem *> selection = _scene->selectedItems();
    foreach (QGraphicsItem *item, selection) {
        NodeItem *nodeItem = dynamic_cast<NodeItem *>(item);
        if (nodeItem){
            nodeItem->beenSelected();
        }
    }
    // TODO if no SublevelNode selected, setSubrange = NULL
    // TODO Allow selection of Connections
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

void GraphWidget::addAllNodes(QList<Node*> allNodes)
{
    // First add all the nodes...
    foreach (Node *node, allNodes)
        addNode(node);

    // ...then make all the connections.
    foreach (Node *node, allNodes) {
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

void GraphWidget::addNode(Node *node)
{
    NodeItem *nodeItem;
    // GROSS
    SublevelNode *sln = dynamic_cast<SublevelNode *>(node);
    if (sln)
        nodeItem = new SublevelNodeItem(node);
    else
        nodeItem = new NodeItem(node);

    // TODO better positioning.
    QPointF  center = _csview->view()->mapToScene(_csview->view()->viewport()->rect().center());
    nodeItem->setPos(center);
    _scene->addItem(nodeItem);
    // Moves the node so it isn't right on top of one that's already displayed.
    nodeItem->avoidCollisions();
}

//TODO  void GraphWidget::frameSelected()
//TODO  void GraphWidget::frameAll()


void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        // XXX this will probably break. need to make sure dtors do right thing.
        deleteSelection();
        update();
        break;
    default:
        qDebug() << Q_FUNC_INFO << "keypress " << event->key();
        QWidget::keyPressEvent(event);
    }
}

void GraphWidget::deleteSelection() {
    QList<QGraphicsItem *> selection = _scene->selectedItems();
    foreach (QGraphicsItem *item, selection) {
        delete item;
    }
}
