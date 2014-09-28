#ifndef PATHNODE_H
#define PATHNODE_H

#include "Node.h"
#include <QObject>

// Nodes that interact with custom view widgets need to inherit from both
//   "QObject" (so they can have signals & slots)
//   "Node" (so they can be used by Cues)
// Note that QObject must come first, as per Qt.

class PathNode : public QObject, public Node
{
    Q_OBJECT

    struct Position {
        int x,y;
    };

public:
    PathNode(QObject *parent = 0);
    void operator() ();
    virtual PathNode* clone();
    void beenSelected();
    void beenDeselected();

    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

signals:
    void pathNodeSelected(PathNode *me);

private:
    Param<int>        _x;       // Output
    Param<int>        _y;       // Output

    QList<Position> _positions;

};

#endif // PATHNODE_H
