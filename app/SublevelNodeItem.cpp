#include "SublevelNodeItem.h"
#include "SublevelNode.h"
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
    proxy->setPos(0, (_node->getParams().size()+1)*guisettings->m_PIheight);
}

QRectF SublevelNodeItem::boundingRect() const
{
    // + 1 for the node name
    int nRows = _node->getParams().size() + 1;
    QRectF bbox;
    if (isMinimized())
        bbox =  QRectF(0,0,guisettings->m_NNWidth, guisettings->m_PIheight);
    else
        bbox =  QRectF(0,0,guisettings->m_NNWidth, guisettings->m_PIheight*nRows + guisettings->sl_barHeight);
    return bbox.marginsAdded(_margins);
}

void SublevelNodeItem::minimize(int status)
{
    if (status)
        _slm->hide();
    else
        _slm->show();
    NodeItem::minimize(status);
}

// -----------------------------------------------------------------------------
// RangeMeter

RangeMeter::RangeMeter(QWidget *parent)
    :   QWidget(parent),
      _level(0.0)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(guisettings->m_NNWidth, guisettings->sl_barHeight);
}

RangeMeter::~RangeMeter() {  }

void RangeMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), guisettings->sl_bgColor);

    // Draw bar
    QRect bar = rect();
    bar.setRight(rect().left() +  _level * rect().width());
    painter.fillRect(bar, guisettings->sl_barColor);

    // Draw pulsar
    QRect squareRect = rect();
    squareRect.setLeft(rect().right()-rect().height());
    QColor pulseColor;
    double clampedLevel = clamp(0.,1.0, _level);
    pulseColor.setHsvF(0.0, 0.0, clampedLevel);
    painter.fillRect(squareRect, pulseColor);
    painter.drawRect(squareRect);
}

void RangeMeter::levelChanged(qreal level)
{
    _level = level;
    update();
}
