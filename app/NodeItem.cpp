#include "NodeItem.h"
#include "CuesheetScene.h"
#include "GuiSettings.h"
#include "utils.h"
#include <QtWidgets>
#include <QPainterPath>
#include <QGraphicsProxyWidget>
#include "ConnectorItem.h"

// Classes for drawing the graph view representation of all the nodes.

//-----------------------------------------------------------------------------
// NodeItem
NodeItem::NodeItem(Node *node, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    _node(node),
    _minimized(false)
{
    _margins = QMarginsF(9,5,9,5);
    setFlags(ItemIsSelectable | ItemIsMovable);

    // Name editor
    QLineEdit *nameEdit = new QLineEdit;
    nameEdit->setText(_node->getName());
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
    proxy->setWidget(nameEdit);
    proxy->setPos(GuiSettings::socketWidth + GuiSettings::paramTextOffset, 5);  // Hardw
    proxy->resize(GuiSettings::nodeWidth*.75, GuiSettings::paramHeight* .75);  // Hardw
    CHECKED_CONNECT(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(nameEdit(QString)));

    // Minimize checkbox
    QCheckBox *cb = new QCheckBox;
    cb->setChecked(_minimized);
    QGraphicsProxyWidget *cbproxy = new QGraphicsProxyWidget(this);
    cbproxy->setWidget(cb);
    qreal extra = GuiSettings::paramHeight - cbproxy->rect().height();
    int yshift =  extra/2;
    cbproxy->setPos(GuiSettings::socketWidth/2, yshift);  // Hardw
    CHECKED_CONNECT(cb, SIGNAL(stateChanged(int)), this, SLOT(minimize(int)));

    // Make a ParamItem for every param in the node:
    int y = 0;
    for (ParamBase *param : node->getParams()) {
        y+=GuiSettings::paramHeight;
        ParamItem *parItem = new ParamItem(param, this);
        parItem->setPos(0,y);
    }
}

NodeItem::~NodeItem()
{
    // Look for connections, and remove them:
    QList<QGraphicsItem *> kiddos = childItems();
    foreach (QGraphicsItem *kid, kiddos) {
        ParamItem* pi = dynamic_cast<ParamItem*>(kid);
        if (! pi) {
            // ErrorHandling
            qDebug() << Q_FUNC_INFO << "hmm, non-paramitem child of nodeitem";
            continue;
        }
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
    delete _node;
}

QRectF NodeItem::boundingRect() const
{
    int nRows = _node->getParams().size() + 1;
    QRectF bbox =  QRectF(0,0,GuiSettings::nodeWidth,
                          GuiSettings::paramHeight*nRows);
    return bbox.marginsAdded(_margins);
}

void NodeItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();

    //painter->setBrush(dragOver ? color.light(130) : GuiColors::nodeBGColor);
    // TODO hover highlighting
    painter->setBrush(GuiSettings::nodeBGColor); // TODO color by type

    QBrush b = painter->brush();
    QPen p = painter->pen();

    // Draw rectangle of entire node:
    QRectF bigrect = boundingRect();
    bigrect = bigrect.marginsRemoved(_margins);
    painter->setBrush(GuiSettings::nodeNameColor);
    painter->drawRect(bigrect);
    painter->setBrush(b);

    if (isSelected()) {
        // Draw bright red rect around selected node.
        painter->save();
        painter->setBrush(QBrush());
        QPen selectedPen = QPen(GuiSettings::selectedNodePenColor);
        selectedPen.setWidth(GuiSettings::selectedNodePenWidth);
        painter->setPen(selectedPen);
        QRectF biggerRect = bigrect.marginsAdded(QMarginsF(2,2,2,2));
        painter->drawRect(biggerRect);
        painter->restore();
    }

    painter->restore();
}

// ---------------------------
// Event handling
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
        delete this->_node;
        this->scene()->removeItem(this);
        deleteLater();
        update();
        break;
    default:
        QGraphicsItem::keyPressEvent(event);
    }
}

// Forward selection to node, in case there's anything special to do
// (e.g. Sublevel nodes need to interact with the spectrograph;
//  path nodes will interact with the floor display.)
void NodeItem::beenSelected()
{
    _node->beenSelected();
}

void NodeItem::beenDeselected()
{
    _node->beenDeselected();
}

void NodeItem::nameEdit(QString newname)
{
    // Since Nodes are not QObjects, we can't send a signal to them directly,
    // so we need the NodeItem to have the edit slot.
    _node->setName(newname);
}

// ---------------------------
// Display
// reposition node item, and signal to connections that we've moved.
void NodeItem::rePos(const QPointF &pos)
{
    setPos(pos);
    // Used for updating any attached connectors.
    emit nodeMovedEventSignal();
}

// Find an empty space in the graph to display a new node:
void NodeItem::avoidCollisions()
{
   QList<QGraphicsItem *>colliders = collidingItems(Qt::IntersectsItemBoundingRect);

   foreach (QGraphicsItem *item, colliders) {
       NodeItem *ni = dynamic_cast<NodeItem *>(item);
       if (ni) {
           moveBy(GuiSettings::nodeSpacing, 0);
           avoidCollisions();
           break;
       }
   }
}

void NodeItem::minimize(int status)
{
    _minimized = status;
    // TODO Hide all paramitems:

    update();
}

// ---------------------------
// Utility

// Another $$$ routine that probably isn't a big deal.
QList<NodeItem*> NodeItem::getUpstreamNodeItems()
{
    QList<NodeItem*> upstream;
    for (ParamBase *param : _node->getParams()) {
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

// ---------------------------
// Serialization
void NodeItem::readFromJSONObj(const QJsonObject &json)
{
    // ErrorHandling
    qreal x, y;

    if (json.contains("uisettings")) {
        QJsonObject jsonUiSettings = json["uisettings"].toObject();
        x = jsonUiSettings["xPos"].toDouble();
        y = jsonUiSettings["yPos"].toDouble();
        setPos(x,y);
    }
}

void NodeItem::writeToJSONObj(QJsonObject &json) const
{
    // ErrorHandling
    QJsonObject jsonUiSettings;
    jsonUiSettings["xPos"] = x();
    jsonUiSettings["yPos"] = y();
    json["uisettings"] = jsonUiSettings;
}

//-----------------------------------------------------------------------------
// ParamItem
ParamItem::ParamItem(ParamBase *param, QGraphicsObject *parent) :
    QGraphicsObject(parent),
    _param(param)
{
    int yOffset = GuiSettings::paramHeight/2;

    // Build socket items
    _socket = new SocketItem(param, this);
    if (param->isOutput())
        // Output items get their socket on the right side
        _socket->setPos(GuiSettings::nodeWidth - GuiSettings::socketWidth, yOffset);
    else {
        // Input items get their socket on the left side, and get an editor
        // proxyWidget too.
        _socket->setPos(GuiSettings::socketWidth,yOffset);

        // Display an editor widget for the parameter value.
        QWidget* paramEditorWidget = param->getEditorWidget(this);

        // Proxy widget will automatically be added to the scene because
        // it's a child of an object in the scene.
        QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
        proxy->setWidget(paramEditorWidget);
        qreal extra = GuiSettings::paramHeight - proxy->rect().height();
        int yshift =  extra/2;
        int xshift = GuiSettings::socketWidth + GuiSettings::paramTextOffset + GuiSettings::paramEditorOffset;
        proxy->setPos(xshift, yshift);

#if 0
        // XXX this is broken; for some reason, it's setting the value on other connections
        // as well.
        if (param->connectedParam())    //TODO update this when connections happen
            parItem->setEnabled(false);
#endif
    }
}

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
    if (_param->isOutput())
        painter->setBrush(GuiSettings::outputParamFillColor);
    else
        painter->setBrush(GuiSettings::paramFillColor);
    QRect rr(0,0,GuiSettings::nodeWidth,GuiSettings::paramHeight);

    // Draw rectangle
    painter->setPen(Qt::black);
    painter->drawRoundedRect(rr, 10, 10);

    // Draw name
    painter->setPen(GuiSettings::paramTextColor);
    rr.translate(GuiSettings::socketWidth + GuiSettings::paramTextOffset, 0);
    painter->setFont(GuiSettings::paramTextFont);
    painter->drawText(rr,Qt::AlignLeft | Qt::AlignVCenter, _param->getName());
    painter->restore();
}

// ---------------------------
// setValue callbacks
//      It would be nice to template these, but that won't work with QObject
//      derived classes and the Qt moc.  Fortunately, it's not a lot of stuff.
//
void ParamItem::setValue(double val) {
    Param<float> *p = dynamic_cast<Param<float> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParentNode()->paramHasBeenEdited();
}

void ParamItem::setValue(int val) {
    Param<int> *p = dynamic_cast<Param<int> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParentNode()->paramHasBeenEdited();
}

void ParamItem::setValue(Lightcolor val) {
    Param<Lightcolor> *p = dynamic_cast<Param<Lightcolor> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParentNode()->paramHasBeenEdited();
}

// For use with QLineEdit
void ParamItem::textChanged(QString text)
{
    Param<int> *pi = dynamic_cast<Param<int> *>(this->_param);
    if (pi) {
        int val = text.toInt();
        pi->setValue(val);
        pi->getParentNode()->paramHasBeenEdited();
        return;
    }

    Param<float> *pf = dynamic_cast<Param<float> *>(this->_param);
    if (pf) {
        float val = text.toFloat();
        pf->setValue(val);
        pf->getParentNode()->paramHasBeenEdited();
        return;
    }

    Q_ASSERT(false);
    // ErrorHandling
}

// Need to have this be a separate "setBoolFunction" with an int argument,
// rather than simply "setValue(bool val)", because Qt checkboxes return
// ints--Checked, unchecked, and "Partially Checked" when in a hierarchy.
void ParamItem::setBoolValue(int val) {
    Param<bool> *p = dynamic_cast<Param<bool> *>(this->_param);
    Q_ASSERT(p);
    if (val == 0)
        p->setValue(false);
    else
        p->setValue(true);

    p->getParentNode()->paramHasBeenEdited();
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
    // Start drawing out a connector line
    qobject_cast<CuesheetScene*>(scene())->startLine(event, this);
    update();
}
