#ifndef SUBLEVELNODEITEM_H
#define SUBLEVELNODEITEM_H
#include "NodeItem.h"

class SublevelNode;
class SublevelMeter;

class SublevelNodeItem : public NodeItem
{
public:
    explicit SublevelNodeItem(Node *node, QGraphicsItem *parent = 0);

private:
    // Aiiie!
    SublevelMeter *_slm;
    SublevelNode *_sln;
};

#endif // SUBLEVELNODEITEM_H
