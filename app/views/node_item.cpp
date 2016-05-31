// NodeItem.cpp
// Classes for drawing the graph view representation of all the nodes.

#include <QtWidgets>
#include <QPainterPath>
#include <QGraphicsProxyWidget>
#include "views/connector_item.h"
#include "views/node_item.h"
#include "views/cuesheet_scene.h"
#include "views/gui_settings.h"
#include "engine/utils.h"


//-----------------------------------------------------------------------------
// NodeItem
NodeItem::NodeItem(Node *node, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    m_node(node),
    m_minimized(false)
{
    node->setNodeItem(this);
    setZValue(-100.);

    m_margins = QMarginsF(9,5,9,5);
    setFlags(ItemIsSelectable | ItemIsMovable  | ItemSendsScenePositionChanges);

    // Name editor
    QLineEdit *nameEdit = new QLineEdit;
    nameEdit->setText(m_node->getName());
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
    proxy->setWidget(nameEdit);
    proxy->setPos(guisettings->m_socketWidth + guisettings->m_paramTextOffset, 2);  // Hardwired
    proxy->resize(guisettings->m_NNWidth*.75, (guisettings->m_PIheight -2)* .75);   // Hardwired
    CHECKED_CONNECT(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(nameEdit(QString)));

    // Minimize checkbox
    QCheckBox *cb = new QCheckBox;
    cb->setChecked(m_minimized);
    QGraphicsProxyWidget *cbproxy = new QGraphicsProxyWidget(this);
    cbproxy->setWidget(cb);
    qreal extra = guisettings->m_PIheight - cbproxy->rect().height();
    int yshift =  extra/2;
    cbproxy->setPos(guisettings->m_socketWidth/2, yshift);  // Hardw
    CHECKED_CONNECT(cb, SIGNAL(stateChanged(int)), this, SLOT(minimize(int)));

    // Make a ParamItem for every param in the node:
    int y = 0;
    for (ParamBase *param : node->getParams()) {
        y+=guisettings->m_PIheight;
        ParamItem *parItem = new ParamItem(param, this);
        parItem->setPos(0,y);
    }
}

NodeItem::~NodeItem()
{
    // Look for connections, and remove them:
    QList<ParamItem*> kiddos = getChildParamItems();
    foreach (ParamItem *pi, kiddos) {
        ConnectorItem *cnctr = csScene()->getConnectorForParam(pi->getParam());
        if (cnctr) {
            // Only need to disconnect clients; if this is a server,
            // the whole node gets disconnected below when the node is deleted.
            cnctr->getClient()->getParam()->disconnect();
            scene()->removeItem(cnctr);
            delete cnctr;
        }
    }

    scene()->removeItem(this);
    delete m_node;
}

QRectF NodeItem::boundingRect() const
{
    int nRows;
    if (m_minimized)
        nRows = 1;
    else
        nRows = m_node->getParams().size() + 1;
    QRectF bbox =  QRectF(0,0,guisettings->m_NNWidth + 2,
                          guisettings->m_PIheight*nRows + 2);
    return bbox.marginsAdded(m_margins);
}

void NodeItem::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();

    // TODO hover highlighting
    //painter->setBrush(dragOver ? color.light(130) : GuiColors::nodeBGColor);
    painter->setBrush(guisettings->m_NNBGColor);

    QBrush b = painter->brush();
    QPen p = painter->pen();

    // Draw rectangle of entire node:
    QRectF bigrect = boundingRect();
    bigrect = bigrect.marginsRemoved(m_margins);

    //Draw dropshadow first
    QRectF shadowrect = bigrect;
    shadowrect.moveTopLeft(QPointF(2.,2.));
    painter->setBrush(QColor(0,0,0));
    painter->drawRoundedRect(shadowrect, guisettings->m_NNborderRadius, guisettings->m_NNborderRadius);

    painter->setBrush(guisettings->m_NNBGColor);
    painter->drawRoundedRect(bigrect, guisettings->m_NNborderRadius, guisettings->m_NNborderRadius);
    painter->setBrush(b);

    if (isSelected()) {
        // Draw bright red rect around selected node.
        painter->save();
        painter->setBrush(QBrush());

        // Make penWidth constant in screen space
        const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
        const qreal penWidth = qreal(guisettings->m_NNselectedWidth) / lod;
        QPen selectedPen = QPen(guisettings->m_NNselectedColor);
        selectedPen.setWidth(penWidth);
        painter->setPen(selectedPen);

        QRectF biggerRect = bigrect.marginsAdded(QMarginsF(2,2,2,2));
        int b = guisettings->m_NNselectedBorderRadius;
        painter->drawRoundedRect(biggerRect, b, b);
        painter->restore();
    }

    painter->restore();
}

// Event handling
//
QVariant    NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemScenePositionHasChanged) {
        emit nodeMovedEventSignal();
    }
    return QGraphicsItem::itemChange(change, value);
}

// Forward selection to node, in case there's anything special to do
// (e.g. Sublevel nodes need to interact with the spectrograph;
//  path nodes will interact with the floor display.)
void NodeItem::beenSelected()
{
    m_node->beenSelected();
}

void NodeItem::beenDeselected()
{
    m_node->beenDeselected();
}

void NodeItem::nameEdit(QString newname)
{
    // Since Nodes are not QObjects, we can't send a signal to them directly,
    // so we need the NodeItem to have the edit slot.
    m_node->setName(newname);
}

// Display
//
// Reposition the node item, and signal to connections that we've moved.
// This is called from routines like align and layout, that reposition
// a NodeItem programmatically.
void NodeItem::rePos(const QPointF &pos)
{
    setPos(pos);
    // Used for updating any attached connectors.
    emit nodeMovedEventSignal();
}

void NodeItem::findEmptySpaceInGraph()
{
   QList<QGraphicsItem *>colliders = collidingItems(Qt::IntersectsItemBoundingRect);

   foreach (QGraphicsItem *item, colliders) {
       NodeItem *ni = dynamic_cast<NodeItem *>(item);
       if (ni) {
           moveBy(guisettings->m_NNSpacing, 0);
           findEmptySpaceInGraph();
           break;
       }
   }
}

void NodeItem::minimize(int status)
{
    m_minimized = status;
    QList<ParamItem*> kiddos = getChildParamItems();
    // Hide all paramitems:
    foreach (ParamItem *pi, kiddos) {
        if (m_minimized)
            pi->hide();
        else
            pi->show();
    }
    emit nodeMovedEventSignal();
    update();
}

// Utility
QList<NodeItem*> NodeItem::getUpstreamNodeItems()
{
    // This could be optimized, but really isn't a big deal.
    QList<NodeItem*> upstream;
    for (ParamBase *param : m_node->getParams()) {
        ParamBase *cnctParam = param->connectedParam();
        if (cnctParam) {
            Node *cnctNode = cnctParam->getParentNode();
            Q_ASSERT(cnctNode);
            NodeItem *cnctNodeItem = this->csScene()->getNodeItemForNode(cnctNode);
            upstream << cnctNodeItem;
        }
    }

    return upstream;
}

QList<ParamItem*> NodeItem::getChildParamItems()
{
    QList<ParamItem*> out;
    QList<QGraphicsItem *> kiddos = childItems();
    foreach (QGraphicsItem *kid, kiddos) {
        ParamItem* pi = dynamic_cast<ParamItem*>(kid);
        if (pi)
            out << pi;
    }
    return out;
}

// Serialization
void NodeItem::readFromJSONObj(const QJsonObject &json)
{
    // ErrorHandling

    if (json.contains("uisettings")) {
        qreal x, y;
        bool minimized = false;
        QJsonObject jsonUiSettings = json["uisettings"].toObject();
        x = jsonUiSettings["xPos"].toDouble();
        y = jsonUiSettings["yPos"].toDouble();
        setPos(x,y);
        minimized = jsonUiSettings["minimized"].toBool();
        minimize(minimized);
    }
}

void NodeItem::writeToJSONObj(QJsonObject &json) const
{
    // ErrorHandling
    QJsonObject jsonUiSettings;
    jsonUiSettings["xPos"] = x();
    jsonUiSettings["yPos"] = y();
    jsonUiSettings["minimized"] = m_minimized;
    json["uisettings"] = jsonUiSettings;
}


//-----------------------------------------------------------------------------
// ParamItem
// ParamItems display the individual parameters of a node.

ParamItem::ParamItem(ParamBase *param, QGraphicsObject *parent) :
    QGraphicsObject(parent),
    m_param(param)
{
    setZValue(-90.);
    int yOffset = guisettings->m_PIheight/2;

    this->setObjectName(tr("ack"));
    // Build socket items
    m_socket = new SocketItem(param, this);
    if (param->isOutput())
        // Output items get their socket on the right side
        m_socket->setPos(guisettings->m_NNWidth - guisettings->m_socketWidth, yOffset);
    else {
        // Input items get their socket on the left side, and get an editor
        // proxyWidget too.
        m_socket->setPos(guisettings->m_socketWidth,yOffset);

        // Display an editor widget for the parameter value.
        QWidget* paramEditorWidget = param->getEditorWidget(this);

        // Proxy widget will automatically be added to the scene because
        // it's a child of an object in the scene.
        QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
        proxy->setWidget(paramEditorWidget);
        qreal extra = guisettings->m_PIheight - proxy->rect().height();
        int yshift =  extra/2;
        int xshift = guisettings->m_socketWidth + guisettings->m_paramTextOffset + guisettings->m_paramEditorOffset;
        proxy->setPos(xshift, yshift);
    }
}

QRectF ParamItem::boundingRect() const
{
    return QRectF(-5, -5, guisettings->m_NNWidth+9, guisettings->m_PIheight+9);
}


void ParamItem::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);
    painter->save();

    // Set box for this param
    if (m_param->isOutput())
        painter->setBrush(guisettings->m_PIoutputbgcolor);
    else
        painter->setBrush(guisettings->m_PIbgcolor);

    QRect rr(2,0,guisettings->m_NNWidth-4,guisettings->m_PIheight);

    // Draw rectangle
    painter->setPen(QPen(guisettings->m_paramBorderColor,
                         guisettings->m_paramBorderWidth));
    painter->drawRoundedRect(rr, guisettings->m_paramBorderRadius,
                             guisettings->m_paramBorderRadius);

    // Draw name
    painter->setPen(guisettings->m_PIfontcolor);
    painter->setFont(guisettings->m_PIfont);
    int offset = guisettings->m_socketWidth + guisettings->m_paramTextOffset;

    if (m_param->isOutput()) {
        rr.setRight(rr.right() - offset);
        painter->drawText(rr,Qt::AlignRight | Qt::AlignVCenter, m_param->getName());
    } else {
        rr.setLeft(rr.left() + offset);
        painter->drawText(rr,Qt::AlignLeft | Qt::AlignVCenter, m_param->getName());
    }
    painter->restore();
}

// setValue callbacks
//      It would be nice to template these, but that won't work with QObject
//      derived classes and the Qt moc.  Fortunately, it's not a lot of stuff.
//
void ParamItem::setValue(double val) {
    Param<float> *p = dynamic_cast<Param<float> *>(this->m_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParentNode()->paramHasBeenEdited();
}

void ParamItem::setValue(int val) {
    Param<int> *p = dynamic_cast<Param<int> *>(this->m_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParentNode()->paramHasBeenEdited();
}

void ParamItem::setValue(Lightcolor val) {
    Param<Lightcolor> *p = dynamic_cast<Param<Lightcolor> *>(this->m_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParentNode()->paramHasBeenEdited();
}

// For use with QLineEdit
void ParamItem::textChanged(QString text)
{
    Param<int> *pi = dynamic_cast<Param<int> *>(this->m_param);
    if (pi) {
        int val = text.toInt();
        pi->setValue(val);
        pi->getParentNode()->paramHasBeenEdited();
        return;
    }

    Param<float> *pf = dynamic_cast<Param<float> *>(this->m_param);
    if (pf) {
        float val = text.toFloat();
        pf->setValue(val);
        pf->getParentNode()->paramHasBeenEdited();
        return;
    }

    // This should only be called for ints and floats.
    Q_ASSERT(false);
}

// Need to have this be a separate "setBoolFunction" with an int argument,
// rather than simply "setValue(bool val)", because Qt checkboxes return
// ints--Checked, unchecked, and "Partially Checked" when in a hierarchy.
void ParamItem::setBoolValue(int val) {
    Param<bool> *p = dynamic_cast<Param<bool> *>(this->m_param);
    Q_ASSERT(p);
    if (val == 0)
        p->setValue(false);
    else
        p->setValue(true);

    p->getParentNode()->paramHasBeenEdited();
}


//-----------------------------------------------------------------------------
// SocketItem
// Sockets are the little squares/dots/whatever that connect nodes.

SocketItem::SocketItem(ParamBase *param, QGraphicsObject *parent) :
    QGraphicsObject(parent),
    m_param(param)
{
    setZValue(-80.);
}

QRectF SocketItem::boundingRect() const {
    QRectF out = QRectF(-guisettings->m_socketWidth/2, -guisettings->m_socketWidth/2,
                         guisettings->m_socketWidth,  guisettings->m_socketWidth);
    out.adjust(-2,-2,2,2);
    return out;
}

void SocketItem::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    painter->save();
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw connector socket
    painter->setBrush(guisettings->m_socketFillColor);
    painter->setPen(guisettings->m_socketOutlinePen);
    painter->drawEllipse(QPointF(0.0, 0.0),
                         guisettings->m_socketWidth,
                         guisettings->m_socketWidth);
    painter->restore();
}

void SocketItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Start drawing out a connector line
    qobject_cast<CuesheetScene*>(scene())->startLine(event, this);
    update();
}

QPointF SocketItem::socketPos()
{
    // Like scene pos, except when minimized.
    if (isVisible()) {      // XXX does this consider parent hidden status?
        return scenePos();
    }

    QPointF nodePos = parentItem()->parentItem()->scenePos();
    QPointF out = scenePos();
    out.setY(nodePos.y() + guisettings->m_PIheight/2);
    return out;
}
