#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QList>
#include <QSet>
#include <QShortcut>

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
    void showAndRaise();
    int  whatToActivate();
    void setAutoSwitchCues(bool status);

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

    QShortcut*          m_frameAllShortcut;
    QShortcut*          m_frameSelectedShortcut;
    QShortcut*          m_layoutAllShortcut;
    QShortcut*          m_xAlignShortcut;
    QShortcut*          m_yAlignShortcut;
    QShortcut*          m_xDistributeShortcut;
    QShortcut*          m_yDistributeShortcut;
    QShortcut*          m_zoomOutShortcut;
    QShortcut*          m_zoomInShortcut;
    QShortcut*          m_zoomResetShortcut;
    QShortcut*          m_duplicateShortcut;
    QShortcut*          m_groupShortcut;
    QShortcut*          m_ungroupShortcut;
    QShortcut*          m_minimizeSelectedShortcut;
    QShortcut*          m_newTabShortcut;

#if 0
    QToolButton *       m_newCueButton;
    QCheckBox *         m_useAllCues;
    QCheckBox *         m_autoSwitchCues;
    QToolButton *       m_segmentButton;
#endif
    bool                m_autoSwitchCues;
    QTabWidget *        m_tabwidget;
    SegmentController * m_segmentController;
    RenameTabDialog*    m_renameTabDialog;

    // Last selection made--used for updating selections
    QSet<NodeItem*>     m_wasSelected;


protected:
    void    keyPressEvent(QKeyEvent *event);
};

#endif // GRAPHWIDGET_H
