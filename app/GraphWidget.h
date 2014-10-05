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
class QTabWidget;
class QToolButton;
class QCheckBox;

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = 0);
    ~GraphWidget() {}

//    void addTheseNodes(QList<Node*> aBunchOfNodes);
    void writeNodeUiToJSONObj(const Node *node, QJsonObject& json);
    void readNodeUiFromJSONObj(Node* node, const QJsonObject &json);

    bool useAllCues();

public slots:
    void addNode(Node *node, QJsonObject* json = nullptr);
    void addConnection(ParamBase *, ParamBase*);
    void selectionChanged();
    void frameAll();
    void xAlign();
    void yAlign();
    void frameSelection();
    void zoomOut();
    void zoomIn();
    void zoomReset();
    void layoutAll();
    void deleteSelection();
    void group();
    void ungroup();
    void duplicate();
    void minimizeSelected();
    void newCuesheet();
    QList<Cue*> getCurrentCues();

private:
    void    connectUi();

    void    align(bool xaxis);
    void    frameItems(QList<QGraphicsItem*> items);
    QPointF positionNodeItem(NodeItem* ni, QPointF startPos);

    CuesheetScene*     getCurrentScene();
    CuesheetView*      getCurrentView();

    QToolButton *       _newCueButton;
    QCheckBox *       _useAllCues;
    QTabWidget *       _tabwidget;
//    CuesheetScene*     _scene;
//    CuesheetView*      _csview;

    // Last selection made--used for updating selections
    QSet<NodeItem*>     _wasSelected;


protected:
    void    keyPressEvent(QKeyEvent *event);
};

#endif // GRAPHWIDGET_H
