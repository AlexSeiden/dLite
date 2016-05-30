#ifndef GROUPNODEITEM_H
#define GROUPNODEITEM_H

#include <QGraphicsItemGroup>

class NodeItem;

class GroupNodeItem : public QGraphicsItemGroup
{
public:
    explicit GroupNodeItem(QGraphicsItem *parent = 0);

    QRectF  boundingRect() const;
    void    paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    QString             getName()                   {return m_name;}
    void                setName(QString name)       {m_name = name;}

protected:
    void        mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
    bool                m_minimized;     // TODO
    QString             m_name;          // TODO
};

#endif // GROUPNODEITEM_H
