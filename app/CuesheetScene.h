#ifndef CUESHEETSCENE_H
#define CUESHEETSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>

// Subclasses QGraphicsScene, for viewing a Cuesheet.
class SocketItem;

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
    explicit CuesheetScene(QObject *parent = 0);
    SocketItem  *getSocket(QGraphicsItem *item);


public slots:
    void setConnecting(bool status=true) {_isConnecting = status;}
    void setStartPoint(QPointF startPoint) {_startPoint = startPoint;}
    void startLine(QGraphicsSceneMouseEvent *mouseEvent, SocketItem *srcItem);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    QGraphicsItem *findFirstReleventItem(QList<QGraphicsItem *> &endItems);
    SocketItem  *_sourceSocket;
    QPointF _startPoint;
    QGraphicsLineItem *_line;
    bool    _isConnecting;
};

#endif // CUESHEETSCENE_H
