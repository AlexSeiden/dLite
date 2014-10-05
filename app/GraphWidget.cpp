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
#include <QTabWidget>
#include <QToolButton>
#include <QCheckBox>
#include <QStyleFactory>
#include "GroupNodeItem.h"
#include "Cue.h"

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent)
{
    _tabwidget = new QTabWidget;
    newCuesheet();
//    qDebug() << QStyleFactory::keys();
    _tabwidget->setStyle(QStyleFactory::create("Fusion"));
//    _tabwidget->setStyle(QStyleFactory::create("Windows"));

    _newCueButton = new QToolButton;
    _newCueButton->setText(tr("+"));
    _newCueButton->setEnabled(true);

    // TODO lots more
    _useAllCues = new QCheckBox;
    _useAllCues->setEnabled(true);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->addWidget(_newCueButton);
    vlayout->addWidget(_useAllCues);
    vlayout->addStretch();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(_tabwidget);
    layout->addLayout(vlayout);
    setLayout(layout);

    connectUi();
    setGeometry(10, 500, 1500, 500);        // hardw
    setWindowTitle("Cuesheet");
    setWindowFlags( Qt::Window | Qt::WindowMaximizeButtonHint |
                    Qt::CustomizeWindowHint);
}

void GraphWidget::connectUi()
{
    CHECKED_CONNECT(_newCueButton, SIGNAL(clicked()), this, SLOT(newCuesheet()));
}

// ------------------------------------------------------------------------------
// Cuesheet mgmt
void GraphWidget::newCuesheet()
{
    int nCuesheets = _tabwidget->count();
    CuesheetView *csv = new CuesheetView(_tabwidget);
    CuesheetScene *css = new CuesheetScene;

    css->setSceneRect(QRectF());
    csv->view()->setScene(css);

    CHECKED_CONNECT(css, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    QString name = QString("Cuesheet %1").arg(nCuesheets);
    _tabwidget->addTab(csv, name);
    css->setName(name);
    _tabwidget->setCurrentWidget(csv);
    update();
}

QList<Cue*> GraphWidget::getCurrentCues()
{
    return getCurrentScene()->getCues();
}

CuesheetScene* GraphWidget::getCurrentScene()
{
    CuesheetView *csv = getCurrentView();
    Q_ASSERT(csv->view());
    QGraphicsScene *scn = csv->view()->scene();
    Q_ASSERT(scn);
    CuesheetScene *csn = qobject_cast<CuesheetScene*>(scn);
    Q_ASSERT(csn);
    return csn;
}

CuesheetView* GraphWidget::getCurrentView()
{
    QWidget *currentTab = _tabwidget->currentWidget();
    Q_ASSERT(currentTab);
    CuesheetView *csv = qobject_cast<CuesheetView*>(currentTab);
    Q_ASSERT(csv);
//    qDebug() << "CurrentIndex" << _tabwidget->currentIndex();
    return csv;
}

bool GraphWidget::useAllCues()
{
    return _useAllCues->isChecked();
}

void GraphWidget::selectionChanged()
{
    // We need to know when items, like SublevelNode, that have special
    // editing are selected, so we can notify other widgets where the
    // editing might take place.  For example, SublevelNodes allow dragging
    // out a rectangle in the spectrograph, and the spectrograph will display
    // the current subrange of a sublevel node.
    // Similarly, path & region nodes use the dancefloor widget.
    QList<QGraphicsItem *> selection = getCurrentScene()->selectedItems();
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

#if 0
// Not used with current reading-from-file paradigm
void GraphWidget::addTheseNodes(QList<Node*> aBunchOfNodes)
{
    // Adds a list of nodes & their connections to the scene.
    // Right now this isn't used; the file read stuff calls
    // addNode & addConnection directly so that NodeItem positions
    // can be restored.

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
#endif

void GraphWidget::addConnection(ParamBase* server, ParamBase* client)
{
    // Find sockets
   SocketItem* serverSocket = getCurrentScene()->getSocketForParam(server);
   SocketItem* clientSocket = getCurrentScene()->getSocketForParam(client);
   if (serverSocket && clientSocket) {
       getCurrentScene()->connectSockets(serverSocket, clientSocket);
   }
   else {
       // ErrorHandling
       qDebug() << Q_FUNC_INFO << "Could not connect" ;
   }
}

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

    Cue *cue = dynamic_cast<Cue*>(node);
    if (cue)
        getCurrentScene()->addCue(cue);

    getCurrentScene()->addItem(nodeItem);

    // When restoring from file, read in positon data:
    if (json)
        nodeItem->readFromJSONObj(*json);
    else {
        // TODO better positioning.
        QPointF center = getCurrentView()->view()->mapToScene(getCurrentView()->view()->viewport()->rect().center());
        nodeItem->rePos(center);

        // Moves the node so it isn't right on top of one that's already displayed.
        nodeItem->avoidCollisions();
        getCurrentScene()->clearSelection();
        nodeItem->setSelected(true);
        getCurrentView()->view()->ensureVisible(nodeItem);
    }
}

// ------------------------------------------------------------------------------
// Serialization
void GraphWidget::writeNodeUiToJSONObj(const Node* node, QJsonObject& json)
{
    NodeItem* ni = getCurrentScene()->getNodeItemForNode(node);
    if (ni)
        ni->writeToJSONObj(json);
    else
        qWarning() << Q_FUNC_INFO << "Could not find NodeItem for " << node->getName();
}

void GraphWidget::readNodeUiFromJSONObj(Node* node, const QJsonObject& json)
{
    NodeItem* ni = getCurrentScene()->getNodeItemForNode(node);
    if (ni)
        ni->readFromJSONObj(json);
    else
        qWarning() << Q_FUNC_INFO << "Could not find NodeItem for " << node->getName();
}

// ------------------------------------------------------------------------------
// Viewing
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
//        qDebug() << Q_FUNC_INFO << "keypress " << event->key() << " ; text()" << event->text();
        event->setAccepted(false);
        QWidget::keyPressEvent(event);
    }
}

void GraphWidget::deleteSelection() {
    QList<QGraphicsItem *> selection = getCurrentScene()->selectedItems();
    foreach (QGraphicsItem *item, selection) {
        delete item;
    }
}

void GraphWidget::zoomOut()
{
    getCurrentView()->zoomOut(10);
    update();
}

void GraphWidget::zoomIn()
{
    getCurrentView()->zoomIn(10);
    update();
}

void GraphWidget::zoomReset()
{
    getCurrentView()->zoomReset();
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
    if (items.length() == 1) {
        getCurrentView()->fitBbox(bbox);
        getCurrentView()->view()->centerOn(items[0]);
    } else
        getCurrentView()->fitBbox(bbox);

}

void GraphWidget::frameSelection()
{
    QList<QGraphicsItem *> selection = getCurrentScene()->selectedItems();
    if (selection.length() == 0) {
        frameAll();
        return;
    }

    frameItems(selection);
}

void GraphWidget::frameAll()
{
    QList<QGraphicsItem *> allItems = getCurrentScene()->items();
    frameItems(allItems);
}

// Automatically layout all nodes.
void GraphWidget::layoutAll()
{
    // Start with Cues, which will be the root of the dag:
    QList<NodeItem*>allCues = getCurrentScene()->getAllCueNodeItems();
    QPointF nextPos = QPointF(2000., -1000.);
    foreach (NodeItem*ni, allCues) {
        QPointF finalPos = positionNodeItem(ni, nextPos);
        nextPos += QPointF(0, ni->boundingRect().height() + GuiSettings::nodeSpacing);
        nextPos.setY(qMax(nextPos.y(), finalPos.y()));
    }

    // TODO orphan nodes (ones not connected) aren't handled.
}

// Recursively layout a node and its connections.
QPointF GraphWidget::positionNodeItem(NodeItem* ni, QPointF startPos)
{
    ni->rePos(startPos);
    // Move to the left
    QPointF nextPos = startPos - QPointF(GuiSettings::nodeWidth + GuiSettings::nodeSpacing/2, 0);

    foreach (NodeItem* upstreamItem, ni->getUpstreamNodeItems()) {
        QPointF finalPos = positionNodeItem(upstreamItem, nextPos);
        nextPos += QPointF(0, upstreamItem->boundingRect().height() + GuiSettings::nodeSpacing);
        nextPos.setY(qMax(nextPos.y(), finalPos.y()));
    }
    return nextPos;
}

// Align selected nodes horizontally or vertically.
void GraphWidget::align(bool xaxis)
{
    QList<NodeItem*> selection = getCurrentScene()->getSelectedNodeItems();
    // Find center:
    qreal center = 0;
    foreach (NodeItem* ni, selection)
        if (xaxis)
            center += ni->pos().x();
        else
            center += ni->pos().y();
    center /= selection.size();

    // Move each node to center along that axis
    foreach (NodeItem* ni, selection) {
        QPointF newpos = ni->pos();
        if (xaxis)
            newpos.setX(center);
        else
            newpos.setY(center);
        ni->rePos(newpos);
    }
}

void GraphWidget::yAlign()
{
    align(false);
}

void GraphWidget::xAlign()
{
    align(true);
}

void GraphWidget::minimizeSelected() {
    QList<NodeItem *> selection = getCurrentScene()->getSelectedNodeItems();
    // Find current minimize status:
    //      If all are minimized, then de-minimize the nodes.
    //      Otherwise, minimize all the nodes.
    bool areMinimized = true;
    foreach (NodeItem* item, selection) {
        if (! item->isMinimized()) {
            areMinimized = false;
            break;
        }
    }

    bool setThemToMinimized = !areMinimized;
    foreach (NodeItem* item, selection)
        item->minimize(setThemToMinimized);
}

// ------------------------------------------------------------------------------
// Dupes & Groups
void GraphWidget::group() {
    QList<QGraphicsItem*> selection = getCurrentScene()->getSelectedGroupableItems();

    // Only group when we have more than one item
    if (selection.size() < 2)
        return;

    GroupNodeItem *grp = new GroupNodeItem();
    getCurrentScene()->addItem(grp);
    foreach (QGraphicsItem *gi, selection)
        grp->addToGroup(gi);
    grp->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    grp->setSelected(true);
}

void GraphWidget::ungroup() {
    QList<GroupNodeItem*> selection = getCurrentScene()->getSelectedGroups();

    foreach (GroupNodeItem *grp, selection)
        getCurrentScene()->destroyItemGroup(grp);
}

void GraphWidget::duplicate() {
    QList<NodeItem *> selection = getCurrentScene()->getSelectedNodeItems();
    // Convert to list of nodes:
//    QList<Node*>* out = new QList<Node*>;
    QList<Node*> out;
    foreach (NodeItem* item, selection) {
        out << item->getNode();
//        (*out) << item->getNode();
    }
    NodeFactory::Singleton()->duplicateNodes(&out);
//    qDebug() << "Nodes Duplicated";
}
