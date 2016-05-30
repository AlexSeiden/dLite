#ifndef REGIONNODE_H
#define REGIONNODE_H

#include "nodes/Node.h"
#include <QObject>
#include <QPoint>
#include "views/dancefloorwidget.h"   // For pixelquerydelegate_t
#include "engine/region.h"

// Nodes that interact with custom view widgets need to inherit from both
//   "QObject" (so they can have signals & slots)
//   "Node" (so they can be used by Cues)
// Note that QObject must come first, per Qt docs.

class RegionNode : public QObject, public Node
{
    Q_OBJECT

public:
    RegionNode(QObject *parent = 0);
    ~RegionNode();
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
    Region* m_regionP;

    bool hasCell(QPoint p);
    void setCell(QPoint p, bool status);
};

#endif // REGIONNODE_H
