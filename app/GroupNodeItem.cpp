#include "GroupNodeItem.h"
#include "NodeItem.h"
#include "GuiSettings.h"
#include <QPainter>
#include <QStyleOption>

GroupNodeItem::GroupNodeItem(QGraphicsItem *parent) :
    QGraphicsItemGroup(parent),
    _minimized(false)
{
    Q_UNUSED(_minimized);
    // TODO add title bar object

    // TODO add minimize checkbox
}

// TODO serialize

QRectF GroupNodeItem::boundingRect() const
{
    QRectF bbox = QGraphicsItemGroup::boundingRect();
    bbox += QMarginsF(4,4+30,4,4);
    return bbox;
}

void GroupNodeItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    painter->save();
    painter->setBrush(QBrush(QColor(200,200,200,100))); // HARDW
    QPen selectedPen;

    if (isSelected()) {
        // Draw bright red rect around selected node.
        // Make penWidth constant in screen space
        const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
        const qreal penWidth = qreal(GuiSettings::selectedNodePenWidth) / lod;
        selectedPen = QPen(GuiSettings::selectedNodePenColor);
        selectedPen.setWidth(penWidth);
    } else {
        selectedPen = QPen(Qt::black);                          // HARDW
        selectedPen.setWidth(GuiSettings::selectedNodePenWidth);
    }
    painter->setPen(selectedPen);
    QRectF bbox = QGraphicsItemGroup::boundingRect();
    bbox += QMarginsF(0,30,0,0); // hardw   for name
    painter->drawRect(bbox);
    painter->restore();

    // don't need or want this:
//    QGraphicsItemGroup::paint(painter, option, widget);
}

void GroupNodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Used for updating any attached connectors.
    foreach (QGraphicsItem *item, childItems()) {
        NodeItem *ni = dynamic_cast<NodeItem *>(item);
        if (ni) {
            ni->nodeMovedEventSignal();
            continue;
        }
        GroupNodeItem *gni = dynamic_cast<GroupNodeItem *>(item);
        if (gni) {
            gni->mouseMoveEvent(event);
            continue;
        }
    }
    QGraphicsItemGroup::mouseMoveEvent(event);
}

