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

    virtual void beenSelected();

signals:
    void nodeMovedEventSignal();

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    bool dragOver;

private:
    Node    *_node;
};


class ParamItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ParamItem(ParamBase *param, QGraphicsObject *parent);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void setSocket(SocketItem *sock) {_socket = sock;}
    SocketItem *getSocket() {return _socket;}

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

private:
    ParamBase    *_param;
};


class ConnectorItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit  ConnectorItem(SocketItem *serverSocket, SocketItem *clientSocket, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void updatePath();

public slots:
    void gotMoved();

private:
    SocketItem    *_serverSocket;
    SocketItem    *_clientSocket;
    QPointF        _pStart;
    QPointF        _pEnd;
    QPainterPath  *_path;
};


#endif // NODEITEM_H
