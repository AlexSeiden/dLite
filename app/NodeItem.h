#ifndef NODEITEM_H
#define NODEITEM_H

#include "Param.h"
#include "Node.h"
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QList>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QJsonObject;
QT_END_NAMESPACE

// Forward declarations
class SocketItem;

class NodeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NodeItem(Node *node, QGraphicsItem *parent = 0);
    ~NodeItem();

    virtual QRectF  boundingRect() const;
    void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void    avoidCollisions();
    Node*   getNode() const {return _node;}
    virtual void beenSelected();

    // Serialization
    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

signals:
    void    nodeMovedEventSignal();

protected:
    void    mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void    keyPressEvent(QKeyEvent *event);

    Node    *_node;
    QMarginsF  _margins; // for padding on the bounding rect
};


class ParamItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ParamItem(ParamBase *param, QGraphicsObject *parent);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void setSocket(SocketItem *sock) {_socket = sock;}
    SocketItem *getSocket()          {return _socket;}

private:
    ParamBase    *_param;
    SocketItem   *_socket;
};


class SocketItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit    SocketItem(ParamBase *param, QGraphicsObject *parent);

    QRectF      boundingRect() const;
    void        paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    ParamBase * getParam() {return _param;}

    void        mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    ParamBase   *_param;
};

#endif // NODEITEM_H
