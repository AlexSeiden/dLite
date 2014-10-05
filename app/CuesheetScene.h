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
    QList<NodeItem*>    getAllNodeItems();
    QList<NodeItem*>    getSelectedNodeItems();
    QList<QGraphicsItem*>    getSelectedGroupableItems();
    QList<GroupNodeItem*>    getSelectedGroups();
    QList<NodeItem*>    getAllCueNodeItems();
    void                selectTheseNodes(QList<Node*> selection);

public slots:
    void setConnecting(bool status=true) {_isConnecting = status;}
    void setStartPoint(QPointF startPoint) {_startPoint = startPoint;}
    void startLine(QGraphicsSceneMouseEvent *mouseEvent, SocketItem *srcItem);
    void connectSockets(SocketItem *server, SocketItem *client);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    QGraphicsItem *findFirstReleventItem(QList<QGraphicsItem *> &endItems);

    SocketItem *        _startSocket;
    QPointF             _startPoint;
    QGraphicsLineItem * _line;
    //  This is set to true while we are in the process of attempting to
    //  to drag a connection between two nodes.  That is, the user has clicked
    //  on a socket, but hasn't yet finished the action by clicking on another
    //  socket to connect to.
    bool                _isConnecting;
};

#endif // CUESHEETSCENE_H
