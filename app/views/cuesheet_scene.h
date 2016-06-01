// Copyright (c) 2014-2016 Alex Seiden
//
#ifndef CUESHEETSCENE_H
#define CUESHEETSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>

// Subclasses QGraphicsScene, for viewing a Cuesheet.
// Should only be directly manipulated by graphwidget


// Forward declarations:
class NodeItem;
class SocketItem;
class ConnectorItem;
class Node;
class ParamBase;
class GroupNodeItem;

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QPointF;
class QGraphicsLineItem;
class QColor;
QT_END_NAMESPACE

class RenderNode;

class CuesheetScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit            CuesheetScene(QObject *parent = 0);
    SocketItem*         getSocket(QGraphicsItem *item);
    SocketItem*         getSocketForParam(const ParamBase* param);
    ConnectorItem*      getConnectorForClient(const ParamBase* client);
    ConnectorItem*      getConnectorForParam(const ParamBase* param);
    NodeItem*           getNodeItemForNode(const Node *node);
    QList<NodeItem*>    getAllNodeItems() const;
    QList<NodeItem*>    getSelectedNodeItems();
    QList<QGraphicsItem*>    getSelectedGroupableItems();
    QList<GroupNodeItem*>    getSelectedGroups();
    QList<NodeItem*>    getAllCueNodeItems();
    void                selectTheseNodes(QList<Node*> selection);
    QString             getName() const {return m_name;}
    void                setName(QString name) {m_name = name;}
    void                addCue(RenderNode *cue) {m_cues<<cue;}
    QList<RenderNode*>         getCues() const {return m_cues;}
    QList<Node*>        getNodes() const;
    QRectF              getItemsBBox(QList<QGraphicsItem*> items) const;


//    void readFromJSONObj(const QJsonObject &json);
//    void writeToJSONObj(QJsonObject &json) const;

public slots:
    void setConnecting(bool status=true) {m_isConnecting = status;}
    void setStartPoint(QPointF startPoint) {m_startPoint = startPoint;}
    void startLine(QGraphicsSceneMouseEvent *mouseEvent, SocketItem *srcItem);
    void connectSockets(SocketItem *server, SocketItem *client);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    QGraphicsItem *findFirstReleventItem(QList<QGraphicsItem *> &endItems);

    SocketItem *        m_startSocket;
    QPointF             m_startPoint;
    QGraphicsLineItem * m_line;
    //  This is set to true while we are in the process of attempting to
    //  to drag a connection between two nodes.  That is, the user has clicked
    //  on a socket, but hasn't yet finished the action by clicking on another
    //  socket to connect to.
    bool                m_isConnecting;
    QString             m_name;

    // GROSS business and view logic getting pretty sloppy here:
    QList<RenderNode*>         m_cues;
    QList<Node*>        m_nodes;
};

#endif // CUESHEETSCENE_H
