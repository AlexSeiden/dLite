#include "GraphWidget.h"
#include "NodeItem.h"
#include <QHBoxLayout>
#include <QGraphicsObject>

int GraphWidget::_numNodeItems = 0;

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent),
    _scene(new QGraphicsScene)
{
    _scene->setSceneRect(-3000.,-3000.,6000.,6000.);
//    populateScene();

    Cuesheet *cs = new Cuesheet("Cuesheet");
    cs->view()->setScene(_scene);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(cs);
    this->setMinimumSize(800,600);
    this->setLayout(layout);
}


#include "Node.h" // XXX

void GraphWidget::populateScene()
{
    RandomNode *rn = new RandomNode();
#if 0
    NodeItem *item = new NodeItem(rn);
    item->setPos(0.,0.);
    _scene->addItem(item);

    item = new NodeItem(rn);
    item->setPos(200.,0.);
    _scene->addItem(item);

    item = new NodeItem(rn);
    item->setPos(200.,200.);
    _scene->addItem(item);
#else
    for (int i = 0; i <3; ++i)
        addNode(rn);
#endif
}

void GraphWidget::addNode(Node *node)
{
    NodeItem *item = new NodeItem(node);
    item->setPos(_numNodeItems*130.,-200.);
    _numNodeItems++;
    _scene->addItem(item);
}
