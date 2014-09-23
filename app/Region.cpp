#include "Region.h"
#include <QJsonObject>
#include <QJsonArray>

Region::Region()
{ }

bool Region::hasCell(QPoint p)
{
    return _cells.contains(p);
}

void Region::setCell(QPoint p, bool status)
{
    // $$$ inefficient!!
    _cells.removeAll(p);
    if (status)
        _cells.append(p);
}

// Serialization
void Region::writeToJSONObj(QJsonObject &json) const
{
    QJsonArray regionJsonArray;
    foreach (const QPoint cell, _cells) {
         QJsonObject posJ;
         posJ["x"] = cell.x();
         posJ["y"] = cell.y();
         regionJsonArray.append(posJ);
     }
    json["cells"] = regionJsonArray;
}

void Region::readFromJSONObj(const QJsonObject &json)
{
    QJsonArray positionsArray = json["cells"].toArray();
    for (int i=0; i<positionsArray.size(); ++i) {
        QJsonObject posJsonObject = positionsArray[i].toObject();
        int x = posJsonObject["x"].toInt();
        int y = posJsonObject["y"].toInt();
        _cells.push_back(QPoint(x,y));
        // (Hopefully) this uses copy to push onto QList of cells.
    }
}
