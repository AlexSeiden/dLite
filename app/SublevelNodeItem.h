#ifndef SUBLEVELNODEITEM_H
#define SUBLEVELNODEITEM_H
#include "NodeItem.h"
#include <QWidget>
#include <QPainter>

class SublevelNode;

// A horizontal visual display of the level measured by a SublevelNode.
class RangeMeter : public QWidget
{
    Q_OBJECT

public:
    explicit RangeMeter(QWidget *parent = 0);
    ~RangeMeter();

    void paintEvent(QPaintEvent *event);

public slots:
    void levelChanged(qreal level);

private:
    // Height of level bar.
    float  _level;
};


// The QGraphicsItem that is drawn in the interface, that represents
// a SublevelNode in the Cuesheet display.
class SublevelNodeItem : public NodeItem
{
public:
    explicit SublevelNodeItem(Node *node, QGraphicsItem *parent = 0);

    QRectF  boundingRect() const;
    void    minimize(int status);

private:
    RangeMeter *_slm;
    SublevelNode *_sln;
};

#endif // SUBLEVELNODEITEM_H
