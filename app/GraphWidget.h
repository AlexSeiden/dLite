#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include "Cuesheet.h"

class Node;
class Subrange;

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = 0);

signals:

public slots:
    void addNode(Node *node);
    void subrangeHasChanged(Subrange *subrange);
    void selectionChanged();
    void buttclick();

private:
    QGraphicsScene *_scene;

    static int _numNodeItems;
};

#endif // GRAPHWIDGET_H
