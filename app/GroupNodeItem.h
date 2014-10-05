#ifndef GROUPNODEITEM_H
#define GROUPNODEITEM_H

#include <QGraphicsItemGroup>

class NodeItem;

class GroupNodeItem : public QGraphicsItemGroup
{
public:
    explicit GroupNodeItem(QGraphicsItem *parent = 0);

//    void    minimize(int status);  // TODO
    QRectF  boundingRect() const;
    void    paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    void    addToNodeItemGroup(NodeItem* ni);
//    QList<NodeItem*>    getMembers()                {return _members;}  NUKEME
    QString             getName()                   {return _name;}
    void                setName(QString name)       {_name = name;}

protected:
    void        mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
    bool                _minimized;     // TODO
    QString             _name;
    QList<NodeItem*>    _members;
};

#endif // GROUPNODEITEM_H
