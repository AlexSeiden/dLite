#include <QGraphicsProxyWidget>

#include "views/SublevelNodeItem.h"
#include "nodes/SublevelNode.h"
#include "views/GuiSettings.h"
#include "engine/utils.h"
#include "views/CuesheetScene.h"

// -----------------------------------------------------------------------------
// RangeMeter

RangeMeter::RangeMeter(QWidget *parent)
    :   QWidget(parent),
      m_level(0.0)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(guisettings->m_NNWidth, guisettings->sl_barHeight);
}

RangeMeter::~RangeMeter() {  }

void RangeMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.save();
    painter.fillRect(rect(), guisettings->sl_bgColor);

    // Draw bar
    QRect bar = rect();
    bar.setRight(rect().left() +  m_level * rect().width());
    painter.fillRect(bar, guisettings->sl_barColor);

    // Draw pulsar
    QRect squareRect = rect();
    squareRect.setLeft(rect().right()-rect().height());
    QColor pulseColor;
    double clampedLevel = clamp(0.,1.0, m_level);
    pulseColor.setHsvF(0.0, 0.0, clampedLevel);
    painter.fillRect(squareRect, pulseColor);
    painter.drawRect(squareRect);

    painter.restore();
}

void RangeMeter::levelChanged(qreal level)
{
    m_level = level;
    update();
}


// -----------------------------------------------------------------------------
// SublevelNodeItem

SublevelNodeItem::SublevelNodeItem(Node *node, QGraphicsItem *parent) :
    NodeItem(node, parent)
{
    m_sublevelNode = dynamic_cast<SublevelNode *>(node);
    m_rangeMeter = new RangeMeter();
    CHECKED_CONNECT(m_sublevelNode, SIGNAL(levelChanged(qreal)),
                    m_rangeMeter, SLOT(levelChanged(qreal)));
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
    proxy->setWidget(m_rangeMeter);
    proxy->setPos(0, (m_node->getParams().size()+1)*guisettings->m_PIheight);
}

QRectF SublevelNodeItem::boundingRect() const
{
    // + 1 for the node name
    int nRows = m_node->getParams().size() + 1;
    QRectF bbox;
    if (isMinimized())
        bbox =  QRectF(0,0,guisettings->m_NNWidth, guisettings->m_PIheight);
    else
        bbox =  QRectF(0,0,guisettings->m_NNWidth,
                       guisettings->m_PIheight*nRows + guisettings->sl_barHeight);
    return bbox.marginsAdded(m_margins);
}

void SublevelNodeItem::minimize(int status)
{
    if (status)
        m_rangeMeter->hide();
    else
        m_rangeMeter->show();
    NodeItem::minimize(status);
}

