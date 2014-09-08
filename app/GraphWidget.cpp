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

#include <QPushButton>

int GraphWidget::_numNodeItems = 0;

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent),
    _scene(new CuesheetScene)
{
    _scene->setSceneRect(-3000.,-3000.,6000.,6000.);

    Cuesheet *cs = new Cuesheet(NULL);
    cs->view()->setScene(_scene);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(cs);
    this->setGeometry(10, 500, 700, 500);
    this->setLayout(layout);
    this->setWindowTitle("Cuesheet");
}

void GraphWidget::addNode(Node *node)
{
    NodeItem *nodeItem = new NodeItem(node);
    nodeItem->setPos(_numNodeItems*GuiSettings::nodeWidth*1.3,-200.);
    _numNodeItems++;
    _scene->addItem(nodeItem);

    int y = 0;
    int yOffset = GuiSettings::paramHeight/2;
    // XXX it seems kinda gross that we're doing this here...
    for (ParamBase *param : node->getParams()) {
        y+=GuiSettings::paramHeight;
        ParamItem *parItem = new ParamItem(param, nodeItem);
        parItem->setPos(0,y);

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
}

void GraphWidget::buttclick()
{
    qDebug() << "buttclick";
}
