#include "CuesheetScene.h"
#include <QGraphicsSceneMouseEvent>
#include "GuiColors.h"
#include <QDebug>


CuesheetScene::CuesheetScene(QObject *parent) :
    QGraphicsScene(parent),
    _line(nullptr),
    _isConnecting(false)
{
}

void CuesheetScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "CuesheetScene Press Event";


//    if (mouseEvent->button() != Qt::LeftButton)
//        return;


    if (_isConnecting) {
        qDebug() << "CuesheetScene press is connecting";
        startLine(mouseEvent);
    }
    else
        qDebug() << "CuesheetScene press not connecting";
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void CuesheetScene::startLine(QGraphicsSceneMouseEvent *mouseEvent) {
    setConnecting();
    _startPoint = mouseEvent->scenePos();
    _line = new QGraphicsLineItem(QLineF(_startPoint, _startPoint));
    _line->setPen(QPen(GuiColors::connectorColor, 2));
    addItem(_line);
}

void CuesheetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "CuesheetScene move Event";
    if (_isConnecting && _line) {
        // If we are in the middle of making a connection, update the
        // line position
        QLineF newLine(_line->line().p1(), mouseEvent->scenePos());
        _line->setLine(newLine);
        // need "update()" call here??
    } else {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void CuesheetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "CuesheetScene release Event";
    if (_isConnecting && _line) {
        // Find the QGraphicsItems at the start of the line
        QList<QGraphicsItem *> startItems = items(_line->line().p1());
        // Remove the line itself from this list--we want to look at the other items
        if (startItems.count() && startItems.first() == _line)
            startItems.removeFirst();

        QList<QGraphicsItem *> endItems = items(_line->line().p2());
        if (endItems.count() && endItems.first() == _line)
            endItems.removeFirst();

        // Delete the temporary line we've been dragging out,
        // because we'll replace it with a real line.
        removeItem(_line);
        delete _line;
        qDebug() << "cuesheetscene mouse release event";

#if 0
        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == SocketItem::Type &&
            endItems.first()->type() == SocketItem::Type &&
            startItems.first() != endItems.first()) {

            // Find the two items we are connecting
            SocketItem *startItem = qgraphicsitem_cast<SocketItem *>(startItems.first());
            SocketItem *endItem = qgraphicsitem_cast<SocketItem *>(endItems.first());

            // Create the actual connection object
            ConnectorItem *connection = new ConnectorItem(startItem, endItem);
            connection->setColor(myLineColor);
            startItem->addArrow(connection);
            endItem->addArrow(connection);
            // Arrows are always in back
            connection->setZValue(-1000.0);
            addItem(connection);
            connection->updatePosition();
        }
#endif
    }

    _line = nullptr;
    setConnecting(false);
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
