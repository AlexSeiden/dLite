#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QList>
//#include "CuesheetScene.h"
//#include "CuesheetView.h"
#include "Param.h"

class Node;
class Subrange;
class CuesheetScene;
class CuesheetView;

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = 0);
    ~GraphWidget() {}

    void addAllNodes(QList<Node*> allNodes);

public slots:
    void addNode(Node *node);
    void selectionChanged();
    void frameAll();
    void frameSelection();

    void subrangeHasChanged(Subrange *subrange);

private:
    void frameItems(QList<QGraphicsItem*> items);
    void addConnection(ParamBase *, ParamBase*);
    void deleteSelection();

    CuesheetScene *     _scene;
    CuesheetView *      _csview;

protected:
    void    keyPressEvent(QKeyEvent *event);
};

#endif // GRAPHWIDGET_H
