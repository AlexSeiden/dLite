// Copyright (C) 2014-2016 Alex Seiden 
//
#ifndef REGION_H
#define REGION_H

#include <QPoint>
#include <QList>
#include <QMetaType>

class QJsonObject;

// Encapsulates an arbitrary list of cells, such as user-drawn shapes and patterns.

class Region
{
public:
    Region();

    bool hasCell(QPoint p);
    void setCell(QPoint p, bool status);
    void readFromJSONObj(const QJsonObject &json);
    void writeToJSONObj(QJsonObject &json) const;
    const QList<QPoint> getCells() {return _cells;}

private:
    // Stores a list of all points (in Dancefloor cell space)
    // in the region.
    QList<QPoint> _cells;
};

Q_DECLARE_METATYPE(Region)

#endif // REGION_H
