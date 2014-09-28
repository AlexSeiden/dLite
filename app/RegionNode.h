#ifndef REGIONNODE_H
#define REGIONNODE_H

#include "Node.h"
#include <QObject>
#include <QPoint>
#include "dancefloorwidget.h"   // For pixelquerydelegate_t
#include "Region.h"

// Nodes that interact with custom view widgets need to inherit from both
//   "QObject" (so they can have signals & slots)
//   "Node" (so they can be used by Cues)
// Note that QObject must come first, as per Qt.

class RegionNode : public QObject, public Node
{
    Q_OBJECT

public:
    RegionNode(QObject *parent = 0);
    void operator() ();
    virtual RegionNode* clone();
    void beenSelected();
    void beenDeselected();

    PixelQueryDelegate_t    getQueryDelegate();
    PixelEditDelegate_t     getEditDelegate();

    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;

signals:
    void regionNodeSelected(RegionNode *me);
    void regionNodeDeselected(RegionNode *me);

private:
    Param<Region> _region;

//    Region  _region;

    bool hasCell(QPoint p);
    void setCell(QPoint p, bool status);
};

#endif // REGIONNODE_H
