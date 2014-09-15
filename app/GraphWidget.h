#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include "CuesheetView.h"

class Node;
class Subrange;

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = 0);
    ~GraphWidget() {}

signals:

public slots:
    void addNode(Node *node);
    void selectionChanged();

    void subrangeHasChanged(Subrange *subrange);

private:
    QGraphicsScene *    _scene;
    CuesheetView *      _csview;
};

#endif // GRAPHWIDGET_H
