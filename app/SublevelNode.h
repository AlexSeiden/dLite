#ifndef SUBLEVELNODE_H
#define SUBLEVELNODE_H

#include "Node.h"
#include "Subrange.h"
#include "frequencyspectrum.h"
#include <QObject>


// Nodes that interact with custom view widgets and/or get info
// sent to them from the engine (and this does both) need to inherit from both
//   "QObject" (so they can have signals & slots)
//   "Node" (so they can be used by Cues)
// Note that QObject must come first, as per Qt.

class SublevelNode : public QObject, public Node
{
    Q_OBJECT

public:
    SublevelNode(QObject *parent = 0);
    void operator() ();
    void beenSelected();

    Subrange *getSubrange() {return &_range;}
    virtual void read(const QJsonObject &json);
    virtual void write(QJsonObject &json) const;


public slots:
    void spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum);
    void setSubrange(Subrange *range) {_range = *range;}

signals:
    // used by RangeMeter widget:
    void levelChanged(qreal level);

    // used by Spectrograph widget:
    void displayThisSubrange(const Subrange &subrange);
    void iveBeenSelected(SublevelNode *me);

private:
    void calculateLevel();

    // Calculated by engine
    FrequencySpectrum   _spectrum;

    Param<float>        _output;
    Subrange            _range;
};

#endif // SUBLEVELNODE_H
