#ifndef REGIONNODE_H
#define REGIONNODE_H

#include "Node.h"
#include <QObject>

// Nodes that interact with custom view widgets need to inherit from both
//   "QObject" (so they can have signals & slots)
//   "Node" (so they can be used by Cues)
// Note that QObject must come first, as per Qt.

class RegionNode : public QObject, public Node
{
    struct Position {
        int x,y;
    };

public:
    RegionNode(QObject *parent = 0);
    void operator() ();
    void beenSelected();

    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

signals:
    void regionNodeSelected(RegionNode *me);

private:
    Param<int>        _x;       // Output
    Param<int>        _y;       // Output

    QList<Position> _positions;

};


#endif // REGIONNODE_H
