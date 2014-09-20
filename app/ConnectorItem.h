#ifndef CONNECTORITEM_H
#define CONNECTORITEM_H
#include <QGraphicsObject>

class SocketItem;

class ConnectorItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit  ConnectorItem(SocketItem *serverSocket, SocketItem *clientSocket, QGraphicsItem *parent = 0);
    ~ConnectorItem();

    QRectF          boundingRect() const;
    void            paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPainterPath    shape() const;

    SocketItem*     getClient()     {return _clientSocket;}
    SocketItem*     getServer()     {return _serverSocket;}
    void            updatePath();

public slots:
    void gotMoved();

private:
    SocketItem    *_serverSocket;
    SocketItem    *_clientSocket;
    QPointF        _pStart;
    QPointF        _pEnd;
    QPainterPath  *_path;
};
#endif // CONNECTORITEM_H
