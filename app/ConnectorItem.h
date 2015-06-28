#ifndef CONNECTORITEM_H
#define CONNECTORITEM_H
#include <QGraphicsObject>

class SocketItem;
class ConnectorItem;

class ConnectorStub : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit  ConnectorStub(QGraphicsItem *parent = 0, bool isOutput=true);
    ~ConnectorStub();

    QRectF          boundingRect() const;
    void            paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget = 0);
//    QPainterPath    shape() const;
    void            updatePath();

public slots:
    void gotMoved();

protected:
    bool            m_isOutput;
    QPointF         m_p;
    QPainterPath*   m_path;

    friend class ConnectorItem;
};


class ConnectorItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit  ConnectorItem(SocketItem *serverSocket, SocketItem *clientSocket,
                            QGraphicsItem *parent = 0);
    ~ConnectorItem();

    QRectF          boundingRect() const;
    void            paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget = 0);
    QPainterPath    shape() const;

    SocketItem*     getClient()     {return m_clientSocket;}
    SocketItem*     getServer()     {return m_serverSocket;}
    void            updatePath();

public slots:
    void gotMoved();

private:
    SocketItem*     m_serverSocket;
    SocketItem*     m_clientSocket;
    QPointF         m_pStart;
    QPointF         m_pEnd;
    QPainterPath*   m_path;
    ConnectorStub   m_startStub;
    ConnectorStub   m_endStub;
};
#endif // CONNECTORITEM_H
