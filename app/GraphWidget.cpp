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
#include "SublevelNodeItem.h"
#include "sublevel.h"
#include <QPushButton>

int GraphWidget::_numNodeItems = 0;

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent),
    _scene(new CuesheetScene)
{
    _scene->setSceneRect(-3000.,-3000.,6000.,6000.);
    CHECKED_CONNECT(_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    CuesheetView *cs = new CuesheetView(NULL);
    cs->view()->setScene(_scene);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(cs);
    setGeometry(10, 500, 1500, 500);
    setLayout(layout);
    setWindowTitle("Cuesheet");
    setWindowFlags( Qt::Window | Qt::WindowMaximizeButtonHint |
                    Qt::CustomizeWindowHint);
}

void GraphWidget::selectionChanged()
{
    QList<QGraphicsItem *> selection = _scene->selectedItems();
//    qDebug() << "selectionChanged" << selection;
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
    foreach (QGraphicsItem *item, selection) {
        SublevelNodeItem *nodeItem = dynamic_cast<SublevelNodeItem *>(item);
        int nSublevelNodes = 0;
        if (nodeItem) {
            nSublevelNodes++;
            SublevelNode *node= dynamic_cast<SublevelNode *>(nodeItem->getNode());
            node->setSubrange(subrange);
        }

        if (nSublevelNodes > 1) {
            // eee
            qDebug() << "Error: multiple subranges set in " << Q_FUNC_INFO << nSublevelNodes;
        }
    }
}

void GraphWidget::addNode(Node *node)
{
    NodeItem *nodeItem;
    // GROSS
    SublevelNode *sln = dynamic_cast<SublevelNode *>(node);
    if (sln)
        nodeItem = new SublevelNodeItem(node);
    else
        nodeItem = new NodeItem(node);

    // TODO better positioning.
    nodeItem->setPos(_numNodeItems*GuiSettings::nodeWidth*1.3,-200.);
    _numNodeItems++;
    _scene->addItem(nodeItem);
}

void GraphWidget::buttclick()
{
    qDebug() << "buttclick";
}
