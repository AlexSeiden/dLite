#include "SublevelNodeItem.h"
#include "SublevelNode.h"
#include "sublevel.h"
#include "GuiSettings.h"
#include "utils.h"
#include "CuesheetScene.h"
#include <QGraphicsProxyWidget>

SublevelNodeItem::SublevelNodeItem(Node *node, QGraphicsItem *parent) :
    NodeItem(node, parent)
{
    _sln = dynamic_cast<SublevelNode *>(node);
    _slm = new RangeMeter();
    CHECKED_CONNECT(_sln, SIGNAL(levelChanged(qreal)), _slm, SLOT(levelChanged(qreal)));
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
    proxy->setWidget(_slm);
    proxy->setPos(0, GuiSettings::paramHeight*2);
}

QRectF SublevelNodeItem::boundingRect() const
{
    // + 1 for the node name
    int nRows = _node->getParams().size() + 1;
    QRectF bbox =  QRectF(0,0,GuiSettings::nodeWidth,
                          GuiSettings::paramHeight*nRows + GuiSettings::sl_barHeight);
    return bbox.marginsAdded(_margins);
}

// -----------------------------------------------------------------------------
// RangeMeter

RangeMeter::RangeMeter(QWidget *parent)
    :   QWidget(parent),
      _level(0.0)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(GuiSettings::nodeWidth, GuiSettings::sl_barHeight);
}

RangeMeter::~RangeMeter() {  }

void RangeMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), GuiSettings::sl_bgColor);

    // Draw bar
    QRect bar = rect();
    bar.setRight(rect().left() +  _level * rect().width());
    painter.fillRect(bar, GuiSettings::sl_barColor);

    // Draw pulsar
    QRect squareRect = rect();
    squareRect.setLeft(rect().right()-rect().height());
    QColor pulseColor;
    double clampedLevel = _level;
    clampedLevel = qMin(0.,clampedLevel);
    clampedLevel = qMax(1.,clampedLevel);
    pulseColor.setHsvF(0.0, 0.0, clampedLevel);
    painter.fillRect(squareRect, pulseColor);
    painter.drawRect(squareRect);
}

void RangeMeter::levelChanged(qreal level)
{
    _level = level;
    update();
}
