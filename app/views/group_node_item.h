// Copyright (c) 2014-2016 Alex Seiden
//
#ifndef GROUP_NODE_ITEM_H
#define GROUP_NODE_ITEM_H

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
    bool                m_minimized;
    QString             m_name;
};

#endif // GROUP_NODE_ITEM_H
