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
class SegmentController;
class SegGui;
class SongSegmentation;

QT_BEGIN_NAMESPACE
class QTabWidget;
class QToolButton;
class QCheckBox;
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
    QList<Cue*> getCurrentCues();

    bool useAllCues();

public slots:
    void addNode(Node *node, QJsonObject* json = nullptr);
    void addConnection(ParamBase *, ParamBase*);
    void selectionChanged();
    void frameAll();
    void xAlign();
    void yAlign();
    void xDistribute();
    void yDistribute();
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
    void showSegmentController();
    void newSong(QString filename);
    void setCuesheet(int index);

signals:
    void    segmentationChanged(SongSegmentation *);

private:
    void    connectUi();

    void    align(bool xaxis);
    void    distribute(bool xaxis);
    void    frameItems(QList<QGraphicsItem*> items);
    QPointF positionNodeItem(NodeItem* ni, QPointF startPos);

    CuesheetScene*     getCurrentScene();
    CuesheetView*      getCurrentView();

    QToolButton *       _newCueButton;
    QCheckBox *         _useAllCues;
    QToolButton *       _segmentButton;
    QTabWidget *        _tabwidget;
    SegmentController * _segmentController;
    SegGui *            _segGui;

//    CuesheetScene*     _scene;
//    CuesheetView*      _csview;

    // Last selection made--used for updating selections
    QSet<NodeItem*>     _wasSelected;


protected:
    void    keyPressEvent(QKeyEvent *event);
};

#endif // GRAPHWIDGET_H
