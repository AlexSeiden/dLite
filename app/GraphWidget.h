#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QList>
#include <QSet>

#include "Param.h"

class Node;
class NodeItem;
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

    void addTheseNodes(QList<Node*> aBunchOfNodes);
    void writeNodeUiToJSONObj(const Node *node, QJsonObject& json);
    void readNodeUiFromJSONObj(Node* node, const QJsonObject &json);

public slots:
    void addNode(Node *node, QJsonObject* json = nullptr);
    void addConnection(ParamBase *, ParamBase*);
    void selectionChanged();
    void frameAll();
    void frameSelection();
    void zoomOut();
    void zoomIn();
    void layoutAll();

    void subrangeHasChanged(Subrange *subrange);

private:
    void    frameItems(QList<QGraphicsItem*> items);
    void    deleteSelection();
    QPointF positionNodeItem(NodeItem* ni, QPointF startPos);

    CuesheetScene *     _scene;
    CuesheetView *      _csview;
    QSet<NodeItem*>     _wasSelected;


protected:
    void    keyPressEvent(QKeyEvent *event);
};

#endif // GRAPHWIDGET_H
