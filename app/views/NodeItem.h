#ifndef NODEITEM_H
#define NODEITEM_H

#include "engine/nodeparameters.h"
#include "nodes/Node.h"
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
public:
    explicit NodeItem(Node* node, QGraphicsItem* parent = 0);
    ~NodeItem();

    virtual QRectF      boundingRect() const;
    void                paint(QPainter* painter,
                              const QStyleOptionGraphicsItem* option,
                              QWidget* widget = 0);

    void                findEmptySpaceInGraph();
    Node*               getNode()       const {return m_node;}
    bool                isMinimized()   const {return m_minimized;}
    virtual void        beenSelected();
    virtual void        beenDeselected();
    QList<NodeItem*>    getUpstreamNodeItems();
    QList<ParamItem*>   getChildParamItems();
    void                rePos(const QPointF &pos);

    CuesheetScene* csScene() {return dynamic_cast<CuesheetScene*>(scene());}

    // Serialization
    virtual void    readFromJSONObj(const QJsonObject &json);
    virtual void    writeToJSONObj(QJsonObject &json) const;

public slots:
    void            nameEdit(QString newname);
    virtual void    minimize(int status);

signals:
    void            nodeMovedEventSignal();

protected:
    QVariant        itemChange(GraphicsItemChange change, const QVariant &value);

    Node*           m_node;
    bool            m_minimized;
    QMarginsF       m_margins; // for padding on the bounding rect
};


class ParamItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit        ParamItem(ParamBase* param, QGraphicsObject* parent);

    QRectF          boundingRect() const;
    void            paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    void            setSocket(SocketItem* sock) {m_socket = sock;}
    SocketItem*     getSocket()                 {return m_socket;}
    ParamBase*      getParam()                  {return m_param;}

public slots:
    // It would be nice to template these, but that won't work with QObject
    // derived classes and the Qt moc.
    void            setValue(double val);
    void            setBoolValue(int val);
    void            setValue(int val);
    void            setValue(Lightcolor val);
    void            textChanged(QString val);

private:
    ParamBase*      m_param;
    SocketItem*     m_socket;
    QColor          m_color;
};


class SocketItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit        SocketItem(ParamBase* param, QGraphicsObject* parent);

    QRectF          boundingRect() const;
    void            paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* option,
                          QWidget* widget = 0);
    ParamBase*      getParam() {return m_param;}
    QPointF         socketPos();

    void            mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    ParamBase*      m_param;
};

#endif // NODEITEM_H
