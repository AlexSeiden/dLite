#ifndef REGION_H
#define REGION_H

#include <QPoint>
#include <QList>
#include <QMetaType>

class QJsonObject;

// Encapsulates an arbitrary list of cells.

class Region
{
public:
    Region();

    bool hasCell(QPoint p);
    void setCell(QPoint p, bool status);
    void readFromJSONObj(const QJsonObject &json);
    void writeToJSONObj(QJsonObject &json) const;
    QList<QPoint> getCells() {return _cells;}       // GROSS

private:
    QList<QPoint> _cells;
};

Q_DECLARE_METATYPE(Region)

#endif // REGION_H
