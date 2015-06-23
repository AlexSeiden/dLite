#include <algorithm>    // For sort
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
#include <QPushButton>
#include <QTabWidget>
#include <QToolButton>
#include <QCheckBox>
#include <QStyleFactory>
#include <QShortcut>

#include "GroupNodeItem.h"
#include "Cue.h"
#include "Cupid.h"
#include "SegmentController.h"
#include "Transport.h"
#include "NodeFactory.h"
#include "RenameTabDialog.h"

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent)
{
    m_tabwidget = new QTabWidget(this);
    m_tabwidget->setMovable(true);
    m_tabwidget->setTabShape(QTabWidget::Triangular);
    m_tabwidget->setTabsClosable(true);  // TODO handle event
    CHECKED_CONNECT(m_tabwidget, SIGNAL(tabCloseRequested(int)),
                    this, SLOT(deleteCuesheet(int)));
    CHECKED_CONNECT(m_tabwidget, SIGNAL(tabBarDoubleClicked(int)),
                    this, SLOT(showRenameTabDialog(int)));
    newCuesheet();

    m_newCueButton = new QToolButton;
    m_newCueButton->setText(tr("+"));
    m_newCueButton->setEnabled(true);

    // TODO lots more
    // e.g. compmodes, etc.
    m_useAllCues = new QCheckBox;
    m_useAllCues->setEnabled(true);
    m_useAllCues->setToolTip("Use All Cues");
    m_useAllCues->setChecked(true);

    m_autoSwitchCues = new QCheckBox;
    m_autoSwitchCues->setEnabled(true);
    m_autoSwitchCues->setToolTip("Auto Switch Cues");
    m_autoSwitchCues->setChecked(true);

//    _segmentButton = new QToolButton;
//    _segmentButton->setText(tr("S"));
//    _segmentButton->setEnabled(true);
    m_segmentController = nullptr;
//    _segGui = nullptr;

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->addWidget(m_newCueButton);
    vlayout->addWidget(m_useAllCues);
    vlayout->addWidget(m_autoSwitchCues);
//    vlayout->addWidget(_segmentButton);
    vlayout->addStretch();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_tabwidget);
    layout->addLayout(vlayout);
    setLayout(layout);

    connectUi();
    setGeometry(10, 500, 1500, 500);        // hardw
    setWindowTitle("Cuesheet");
    setWindowFlags( Qt::Window | Qt::WindowMaximizeButtonHint |
                    Qt::CustomizeWindowHint);

    m_renameTabDialog = new RenameTabDialog(this);

    // GROSS this doesn't really belong here in GraphWidget
    // GROSS this duplicates code below.
    m_segmentController = new SegmentController;
    CHECKED_CONNECT(this, SIGNAL(segmentationChanged(SongSegmentation*)),
                    Cupid::Singleton()->getTransport(), SLOT(segmentsChanged(SongSegmentation*)));
    CHECKED_CONNECT(Cupid::Singleton()->getEngine(), SIGNAL(playPositionChanged(qint64)),
                    this, SLOT(whatToActivate()));
    emit segmentationChanged(&(m_segmentController->_segmentation));
    createShortcuts();
}

void GraphWidget::connectUi()
{
    CHECKED_CONNECT(Cupid::Singleton()->getEngine(), SIGNAL(newSong(QString)),
                    this, SLOT(newSong(QString)));
    CHECKED_CONNECT(m_newCueButton, SIGNAL(clicked()), this, SLOT(newCuesheet()));
//    CHECKED_CONNECT(_segmentButton, SIGNAL(clicked()), this, SLOT(showSegmentController()));
}

void GraphWidget::createAppShortcuts()
{
    // ----------------------------------------
    // Graph view shortcuts
    // TODO move these to graph view!
    m_frameAllShortcut = new QShortcut(Qt::Key_A, this);
    m_frameAllShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_frameAllShortcut, SIGNAL(activated()), this, SLOT(frameAll()));

    m_frameSelectedShortcut = new QShortcut(Qt::Key_F, this);
    m_frameSelectedShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_frameSelectedShortcut, SIGNAL(activated()), this, SLOT(frameSelection()));

    m_zoomOutShortcut = new QShortcut(Qt::Key_Minus, this);
    m_zoomOutShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_zoomOutShortcut, SIGNAL(activated()), this, SLOT(zoomOut()));

    m_zoomInShortcut = new QShortcut(Qt::Key_Equal, this);
    m_zoomInShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_zoomInShortcut, SIGNAL(activated()), this, SLOT(zoomIn()));

    m_zoomResetShortcut = new QShortcut(Qt::Key_BracketLeft, this);
    m_zoomResetShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_zoomResetShortcut, SIGNAL(activated()), this, SLOT(zoomReset()));

    m_layoutAllShortcut = new QShortcut(Qt::Key_L, this);
    m_layoutAllShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_layoutAllShortcut, SIGNAL(activated()), this, SLOT(layoutAll()));

    m_xAlignShortcut = new QShortcut(Qt::Key_X, this);
    m_xAlignShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_xAlignShortcut, SIGNAL(activated()), this, SLOT(xAlign()));

    m_yAlignShortcut = new QShortcut(Qt::Key_Y, this);
    m_yAlignShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_yAlignShortcut, SIGNAL(activated()), this, SLOT(yAlign()));

    m_xDistributeShortcut = new QShortcut(QKeySequence("Shift+X"), this);
    m_xDistributeShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_xDistributeShortcut, SIGNAL(activated()), this, SLOT(xDistribute()));

    m_yDistributeShortcut = new QShortcut(QKeySequence("Shift+Y"), this);
    m_yDistributeShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_yDistributeShortcut, SIGNAL(activated()), this, SLOT(yDistribute()));

    m_duplicateShortcut = new QShortcut(Qt::Key_D, this);
    m_duplicateShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_duplicateShortcut, SIGNAL(activated()), this, SLOT(duplicate()));

    m_groupShortcut = new QShortcut(Qt::Key_G, this);
    m_groupShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_groupShortcut, SIGNAL(activated()), this, SLOT(group()));

    m_ungroupShortcut = new QShortcut(Qt::Key_U, this);
    m_ungroupShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_ungroupShortcut, SIGNAL(activated()), this, SLOT(ungroup()));

    m_minimizeSelectedShortcut = new QShortcut(Qt::Key_M, this);
    m_minimizeSelectedShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_minimizeSelectedShortcut, SIGNAL(activated()), this, SLOT(minimizeSelected()));

    m_newTabShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this);
    m_newTabShortcut->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(m_newTabShortcut, SIGNAL(activated()), this, SLOT(newCuesheet()));
}

// ------------------------------------------------------------------------------
// Cuesheet mgmt
CuesheetScene* GraphWidget::newCuesheet(QString name)
{
    CuesheetView *csv = new CuesheetView(m_tabwidget);
    CuesheetScene *css = new CuesheetScene;

    css->setSceneRect(QRectF());
    csv->view()->setScene(css);

    CHECKED_CONNECT(css, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    m_tabwidget->addTab(csv, name);
    css->setName(name);
    m_tabwidget->setCurrentWidget(csv);
    update();
    return css;
}

CuesheetScene* GraphWidget::newCuesheet()
{
    int nCuesheets = m_tabwidget->count();
    QString name = QString("Cuesheet %1").arg(nCuesheets);
    CuesheetScene *css = newCuesheet(name);
    return css;
}

QList<CuesheetScene*> GraphWidget::getCuesheets()
{
    // GROSS oh man really breakin down the view/business wall here
    QList<CuesheetScene*> out;
    for (int i=0; i<m_tabwidget->count(); ++i) {
        CuesheetScene* css = getCurrentScene(i);
        out << css;
    }
    return out;
}

void GraphWidget::deleteCuesheet(int index)
{
    QWidget *widget = m_tabwidget->widget(index);
    CuesheetView *csv = getCurrentView(index);
    CuesheetScene *css = getCurrentScene(index);
    // If deleting the last widget, first create a new one,
    // because we have to have at least one widget
    if (m_tabwidget->count() == 1) {
        newCuesheet();
    }
    delete csv;
    delete css;
    m_tabwidget->removeTab(m_tabwidget->indexOf(widget));
}

void GraphWidget::deleteEmptyFirstCuesheet()
{
    // This is for when we load dLite a file immediately after opening the app.
    // There's already a "Cuesheet 0" that doesn't have anything in it.
    // We delete that sheet.  If there's anything in the sheet, we keep it.
    CuesheetScene *css = getCurrentScene(0);
    if (css->items().size() == 0) {
        deleteCuesheet(0);
    }
}

void GraphWidget::showSegmentController()
{
    if (! m_segmentController) {
        m_segmentController = new SegmentController;
        CHECKED_CONNECT(this, SIGNAL(segmentationChanged(SongSegmentation*)),
                        Cupid::Singleton()->getTransport(), SLOT(segmentsChanged(SongSegmentation*)));
        emit segmentationChanged(&(m_segmentController->_segmentation));
    }
}

int GraphWidget::whatToActivate()
{
    int ms = Cupid::getPlaybackPositionUSecs()/1000;
    int segmentIndex = m_segmentController->findSegment(ms);
//    int cuesheetIndex = _segmentIndexToCuesheetIndex[segmentIndex];
    int cuesheetIndex = segmentIndex % m_tabwidget->count();
    if (autoSwitchCues())
        emit setCuesheet(cuesheetIndex);
    return cuesheetIndex;
}

void GraphWidget::showRenameTabDialog(int index)
{
    m_renameTabDialog->setIndex(index);
    m_renameTabDialog->setTabname(m_tabwidget->tabText(index));
    m_renameTabDialog->exec();
    if (m_renameTabDialog->result() == QDialog::Accepted) {
        // Style: kinda mixed up here...CuesheetScene should probably own
        // the tab name
        m_tabwidget->setTabText(index, m_renameTabDialog->tabname());
        getCurrentScene()->setName(m_renameTabDialog->tabname());
    }
}

QList<Cue*> GraphWidget::getCurrentCues()
{
    // Returns all cues on the current tab
    return getCurrentScene()->getCues();
}

CuesheetScene* GraphWidget::getCurrentScene()
{
    return getCurrentScene(m_tabwidget->currentIndex());
}

CuesheetScene* GraphWidget::getCurrentScene(int i)
{
    // Returns the cuesheetsceen associated with the current tab.
    CuesheetView *csv = getCurrentView(i);
    Q_ASSERT(csv->view());
    QGraphicsScene *scn = csv->view()->scene();
    Q_ASSERT(scn);
    CuesheetScene *csn = qobject_cast<CuesheetScene*>(scn);
    Q_ASSERT(csn);
    return csn;
}

CuesheetView* GraphWidget::getCurrentView()
{
    return getCurrentView(m_tabwidget->currentIndex());
}

CuesheetView* GraphWidget::getCurrentView(int i)
{
    QWidget *currentTab = m_tabwidget->widget(i);
    Q_ASSERT(currentTab);
    CuesheetView *csv = qobject_cast<CuesheetView*>(currentTab);
    Q_ASSERT(csv);
    return csv;
}

bool GraphWidget::useAllCues()
{
    // When "_useAllCues" is false, only cues on the
    // current cuesheet are active.
    return m_useAllCues->isChecked();
}

bool GraphWidget::autoSwitchCues()
{
    return m_autoSwitchCues->isChecked();
}

void GraphWidget::setCuesheet(int index)
{
    if (index < m_tabwidget->count())
        m_tabwidget->setCurrentIndex(index);
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
    QSet<NodeItem*> itemsDeselected = m_wasSelected - newSelection;
    QSet<NodeItem*> itemsNewlySelected = newSelection - m_wasSelected;

    foreach (NodeItem* item, itemsDeselected)
        item->beenDeselected();

    foreach (NodeItem* item, itemsNewlySelected)
        item->beenSelected();

    // TODO if no SublevelNode selected, setSubrange = NULL

    m_wasSelected = newSelection;
}

void GraphWidget::selectNodes(QList<Node *>nodes)
{
    getCurrentScene()->selectTheseNodes(nodes);
}

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
       qDebug() << Q_FUNC_INFO << "Could not connect server "
                << server->getParentNode()->getName() << server->getName()
                << "to client"
                << client->getParentNode()->getName() << client->getName();
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
        nodeItem->findEmptySpaceInGraph();
        getCurrentScene()->clearSelection();
        nodeItem->setSelected(true);
        getCurrentView()->view()->ensureVisible(nodeItem);
    }
}

// ------------------------------------------------------------------------------
// Serialization
void GraphWidget::writeNodeUiToJSONObj(const Node* node, QJsonObject& json)
{
    NodeItem* ni = node->getNodeItem();
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
// TODO layout selection
void GraphWidget::layoutAll()
{
    // Start with Cues, which will be the root of the dag:
    QList<NodeItem*>allCues = getCurrentScene()->getAllCueNodeItems();
    QPointF nextPos = QPointF(2000., -1000.);
    foreach (NodeItem*ni, allCues) {
        QPointF finalPos = positionNodeItem(ni, nextPos);
        nextPos += QPointF(0, ni->boundingRect().height() + guisettings->m_NNSpacing);
        nextPos.setY(qMax(nextPos.y(), finalPos.y()));
    }

    // TODO orphan nodes (ones not connected) aren't handled.
}

// Recursively layout a node and its connections.
QPointF GraphWidget::positionNodeItem(NodeItem* ni, QPointF startPos)
{
    ni->rePos(startPos);
    // Move to the left
    QPointF nextPos = startPos - QPointF(guisettings->m_NNWidth + guisettings->m_NNSpacing/2, 0);

    foreach (NodeItem* upstreamItem, ni->getUpstreamNodeItems()) {
        QPointF finalPos = positionNodeItem(upstreamItem, nextPos);
        nextPos += QPointF(0, upstreamItem->boundingRect().height() + guisettings->m_NNSpacing);
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

bool compareX(NodeItem *lhs, NodeItem *rhs)
{
    return (lhs->pos().x() < rhs->pos().x());
}

bool compareY(NodeItem *lhs, NodeItem *rhs)
{
    return (lhs->pos().y() < rhs->pos().y());
}

// distribute selected nodes horizontally or vertically.
void GraphWidget::distribute(bool xaxis)
{
    QList<NodeItem*> selection = getCurrentScene()->getSelectedNodeItems();
    // Sort nodesitems in selected axis:
    if (xaxis)
        std::sort(selection.begin(), selection.end(), compareX);
    else
        std::sort(selection.begin(), selection.end(), compareY);

    // Move each node to the appropriate position
    QList<NodeItem*>::iterator start = selection.begin();
    QList<NodeItem*>::iterator finish = selection.end();
    finish--;
    qreal minValOnAxis;
    qreal maxValOnAxis;
    if (xaxis) {
        minValOnAxis = (*start)->pos().x();
        maxValOnAxis = (*finish)->pos().x();
    } else {
        minValOnAxis = (*start)->pos().y();
        maxValOnAxis = (*finish)->pos().y();
    }
    qreal spacing = (maxValOnAxis - minValOnAxis)/selection.size();
    start++;

    for (int i=1; start<finish; ++start, ++i) {
        QPointF newpos = (*start)->pos();
        if (xaxis)
            newpos.setX(minValOnAxis + i*spacing);
        else
            newpos.setY(minValOnAxis + i*spacing);
        (*start)->rePos(newpos);
    }
}

void GraphWidget::yDistribute()
{
    distribute(false);
}

void GraphWidget::xDistribute()
{
    distribute(true);
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
    QList<Node*> out;
    foreach (NodeItem* item, selection) {
        out << item->getNode();
    }
    NodeFactory::Singleton()->duplicateNodes(&out);
}

// ------------------------------------------------------------------------------
// Observer forwarding
void GraphWidget::newSong(QString filename)
{
    Q_UNUSED(filename);
    // remember, filename passed into this is audio file,
    // and node loadFile(QString) methods expect the name of file they
    // are supposed to load (e.g. beat onset file).  They will
    // figure out the correct filename in their loadfile method,
    // and will get the new song file path from Cupid.
    foreach (Node* node, Node::allNodes())
        node->loadFile();

    if (m_segmentController)
        m_segmentController->loadFile();
//    if (_segGui)
//        _segGui->setNumCues(_tabwidget->count());
}

#include <QSignalMapper>

// Create shortcuts for switching between tabs
void GraphWidget::createShortcuts() {
    QSignalMapper *sm = new QSignalMapper(this);
    CHECKED_CONNECT(sm, SIGNAL(mapped(int)),
                    this, SLOT(setCuesheet(int)));
    for (int i=0; i<8; ++i) {
        QShortcut *sc = new QShortcut(QKeySequence(QString::number(i+1)), this);
        sm->setMapping(sc, i);
        sc->setContext(Qt::ApplicationShortcut);
        CHECKED_CONNECT(sc, SIGNAL(activated()), sm, SLOT(map()));
    }
    QShortcut *sc = new QShortcut(QKeySequence(QString::number(0)), this);
    sm->setMapping(sc, 9);
    sc->setContext(Qt::ApplicationShortcut);
    CHECKED_CONNECT(sc, SIGNAL(activated()), sm, SLOT(map()));
}
