#include "GraphWidget.h"
#include "NodeItem.h"
#include <QHBoxLayout>
#include <QGraphicsObject>
#include "Node.h"
#include "CuesheetScene.h"

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
    NodeItem *item = new NodeItem(node);
    item->setPos(_numNodeItems*130.,-200.);
    _numNodeItems++;
    _scene->addItem(item);

    int y = 0;
    int yOffset = NodeItem::s_height/2 - SocketItem::s_width/2;
    for (ParamBase *param : node->getParams()) {
        y+=NodeItem::s_height;
        ParamItem *parItem = new ParamItem(param, item);
        parItem->setPos(0,y);
        SocketItem *sockItem = new SocketItem(param, parItem);
        if (param->isOutput())
            sockItem->setPos(NodeItem::s_width-10,yOffset);
        else
            sockItem->setPos(10,yOffset);
    }
}