#ifndef NODEITEM_H
#define NODEITEM_H

#include "Param.h"
#include "Node.h"
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QList>


QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

// Forward declarations
class SocketItem;

class NodeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NodeItem(Node *node, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    static const int s_width=100;
    static const int s_height=25;
    
signals:
    void nodeMovedEventSignal();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    bool dragOver;
    QColor  color;

private:
    Node    *_node;
    QVector<QPointF> stuff; // XXX from chip
};


class ParamItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ParamItem(ParamBase *param, QGraphicsObject *parent);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void setSocket(SocketItem *sock) {_socket = sock;}
    SocketItem *getSocket() {return _socket;}

    static const int s_width=100;
    static const int s_height=25;

private:
    ParamBase    *_param;
    SocketItem   *_socket;

};


class SocketItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit SocketItem(ParamBase *param, QGraphicsObject *parent);

    QRectF boundingRect() const;
    void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    ParamBase *getParam() {return _param;}

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    static const int s_width=16;
    
private:
    ParamBase    *_param;
};


class ConnectorItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit  ConnectorItem(SocketItem *sourceSocket, SocketItem *targetSocket, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void updatePath();

public slots:
    void gotMoved();

private:
    SocketItem    *_sourceSocket;
    SocketItem    *_targetSocket;
    QPointF        _pStart;
    QPointF        _pEnd;
    QPainterPath  *_path;
};


#endif // NODEITEM_H
