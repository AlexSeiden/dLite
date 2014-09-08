#include "SublevelNodeItem.h"
#include "SublevelNode.h"
#include "sublevel.h"
#include "GuiColors.h"
#include "utils.h"
#include "CuesheetScene.h"
#include <QGraphicsProxyWidget>

SublevelNodeItem::SublevelNodeItem(Node *node, QGraphicsItem *parent) :
    NodeItem(node, parent)
{
    _sln = dynamic_cast<SublevelNode *>(node);
    _slm = new SublevelMeter();
    CHECKED_CONNECT(_sln, SIGNAL(levelChanged(qreal)), _slm, SLOT(levelChanged(qreal)));
    QGraphicsProxyWidget *proxy = scene()->addWidget(_slm);
    proxy->setParentItem(this);
    proxy->setPos(GuiSettings::nodeWidth+2, 0);
}
