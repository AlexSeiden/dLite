#ifndef NODEITEM_H
#define NODEITEM_H

#include "Param.h"
#include "Node.h"
#include "CuesheetScene.h"
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QList>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QJsonObject;
QT_END_NAMESPACE

// Forward declarations
class ParamItem;
class SocketItem;
class ConnectionItem;

class NodeItem : public QGraphicsObject
{
    Q_OBJECT
//    Q_PROPERTY(QColor color MEMBER m_color)
//    Q_PROPERTY(QColor background-color MEMBER m_background-color)
//    Q_PROPERTY(QFont font MEMBER m_font)

public:
    explicit NodeItem(Node* node, QGraphicsItem* parent = 0);
    ~NodeItem();

    virtual QRectF  boundingRect() const;
    void    paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    void    avoidCollisions();
    Node*   getNode() const {return _node;}
    bool    isMinimized() const {return _minimized;}
    virtual void beenSelected();
    virtual void beenDeselected();
    QList<NodeItem*>    getUpstreamNodeItems();
    QList<ParamItem*>   getChildParamItems();
    void rePos(const QPointF &pos);

    QList<ConnectionItem*>  getConnectionItems();       // NOTE this isn't implemented

    CuesheetScene* csScene() {return dynamic_cast<CuesheetScene*>(scene());}

    // Serialization
    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

public slots:
    void            nameEdit(QString newname);
    virtual void    minimize(int status);

signals:
    void        nodeMovedEventSignal();

protected:
//    void        mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    QVariant    itemChange(GraphicsItemChange change, const QVariant &value);

//    void        keyPressEvent(QKeyEvent* event);

    Node*       _node;
    bool        _minimized;
    QMarginsF   _margins; // for padding on the bounding rect
};


class ParamItem : public QGraphicsObject
{
    Q_OBJECT
    //Q_PROPERTY(QColor bgcolor MEMBER m_color NOTIFY bgColorChanged DESIGNABLE true)
    Q_PROPERTY(QColor bgcolor READ get_bgcolor WRITE set_bgcolor DESIGNABLE true)

public:
    explicit        ParamItem(ParamBase* param, QGraphicsObject* parent);

    QRectF          boundingRect() const;
    void            paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    void            setSocket(SocketItem* sock) {_socket = sock;}
    SocketItem*     getSocket()                 {return _socket;}
    ParamBase*      getParam()                  {return _param;}

    QColor          get_bgcolor() const;
    void          set_bgcolor(QColor c);
signals:
    void bgColorChanged();

public slots:
    // It would be nice to template these, but that won't work with QObject
    // derived classes and the Qt moc.
    void setValue(double val);
    void setBoolValue(int val);
    void setValue(int val);
    void setValue(Lightcolor val);
    void textChanged(QString val);

private:
    ParamBase*      _param;
    SocketItem*     _socket;
    QColor          m_color;
};


class SocketItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit    SocketItem(ParamBase* param, QGraphicsObject* parent);

    QRectF      boundingRect() const;
    void        paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    ParamBase*  getParam() {return _param;}
    QPointF     socketPos();

    void        mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    ParamBase*  _param;
};

#endif // NODEITEM_H
