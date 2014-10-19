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
class RenameTabDialog;

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

    void writeNodeUiToJSONObj(const Node *node, QJsonObject& json);
    void readNodeUiFromJSONObj(Node* node, const QJsonObject &json);

    QList<Cue*>             getCurrentCues();
    QList<CuesheetScene*>   getCuesheets();
    CuesheetScene*          newCuesheet(QString name);
    bool                    useAllCues();
    bool                    autoSwitchCues();
    void                    selectNodes(QList<Node*>);

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
    CuesheetScene *newCuesheet();
    void showSegmentController();
    void showRenameTabDialog(int index);
    void newSong(QString filename);
    void setCuesheet(int index);
    void deleteCuesheet(int index);
    void deleteEmptyFirstCuesheet();
    int whatToActivate();

signals:
    void segmentationChanged(SongSegmentation *);

private:
    void    connectUi();
    void    createShortcuts();

    void    align(bool xaxis);
    void    distribute(bool xaxis);
    void    frameItems(QList<QGraphicsItem*> items);
    QPointF positionNodeItem(NodeItem* ni, QPointF startPos);

    CuesheetScene*     getCurrentScene(int i);
    CuesheetScene*     getCurrentScene();
    CuesheetView*      getCurrentView(int i);
    CuesheetView*      getCurrentView();

    QToolButton *       _newCueButton;
    QCheckBox *         _useAllCues;
    QCheckBox *         _autoSwitchCues;
    QToolButton *       _segmentButton;
    QTabWidget *        _tabwidget;
    SegmentController * _segmentController;
//    SegGui *            _segGui;
    RenameTabDialog*    _renameTabDialog;

    // Last selection made--used for updating selections
    QSet<NodeItem*>     _wasSelected;


protected:
    void    keyPressEvent(QKeyEvent *event);
};

#endif // GRAPHWIDGET_H
