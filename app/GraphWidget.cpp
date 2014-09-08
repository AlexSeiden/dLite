#include "GraphWidget.h"
#include "NodeItem.h"
#include "Node.h"
#include "CuesheetScene.h"
#include "GuiColors.h"
#include "utils.h"
#include "ParamView.h"
#include <QHBoxLayout>
#include <QGraphicsObject>
#include <QGraphicsProxyWidget>

#include "SublevelNode.h"
#include "sublevel.h"
#include <QPushButton>

int GraphWidget::_numNodeItems = 0;

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent),
    _scene(new CuesheetScene)
{
    _scene->setSceneRect(-3000.,-3000.,6000.,6000.);
    CHECKED_CONNECT(_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    Cuesheet *cs = new Cuesheet(NULL);
    cs->view()->setScene(_scene);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(cs);
    this->setGeometry(10, 500, 700, 500);
    this->setLayout(layout);
    this->setWindowTitle("Cuesheet");
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
}

void GraphWidget::subrangeHasChanged(Subrange *subrange)
{
    QList<QGraphicsItem *> selection = _scene->selectedItems();
#if 0  // XXX
    foreach (QGraphicsItem *item, selection) {
        SublevelNodeItem *nodeItem = dynamic_cast<SublevelNodeItem *>(item);
        if (nodeItem){
            nodeItem->setRange(subrange);
        }
    }
#endif
}

void GraphWidget::addNode(Node *node)
{
    NodeItem *nodeItem = new NodeItem(node);
    // XXX it seems kinda gross that we're doing this here...


    // TODO better positioning.
    nodeItem->setPos(_numNodeItems*GuiSettings::nodeWidth*1.3,-200.);
    _numNodeItems++;
    _scene->addItem(nodeItem);

#if 0
    // Build ParamItems within node
    int y = 0;
    int yOffset = GuiSettings::paramHeight/2;
    for (ParamBase *param : node->getParams()) {
        y+=GuiSettings::paramHeight;
        ParamItem *parItem = new ParamItem(param, nodeItem);
        parItem->setPos(0,y);

        // And build socket items within each param.
        SocketItem *sockItem = new SocketItem(param, parItem);
        parItem->setSocket(sockItem);
        if (param->isOutput())
            sockItem->setPos(GuiSettings::nodeWidth - GuiSettings::socketWidth, yOffset);
        else
            sockItem->setPos(GuiSettings::socketWidth,yOffset);

        ParamView *pv = new ParamView(nullptr, param);
        QGraphicsProxyWidget *proxy = _scene->addWidget(pv);
        proxy->setParentItem(parItem);
        proxy->setPos(GuiSettings::socketWidth * 2 +70, 0);
    }
#endif

    // XXX
    SublevelNode *sln = dynamic_cast<SublevelNode *>(node);
    if (sln) {
        SublevelMeter *slm = new SublevelMeter();
        QGraphicsProxyWidget *proxy = _scene->addWidget(slm);
        proxy->setParentItem(nodeItem);
        proxy->setPos(GuiSettings::nodeWidth+2, 0);
    }

}

void GraphWidget::buttclick()
{
    qDebug() << "buttclick";
}
