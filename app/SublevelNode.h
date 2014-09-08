#ifndef SUBLEVELNODE_H
#define SUBLEVELNODE_H

#include "Node.h"
#include "Subrange.h"
#include "frequencyspectrum.h"
#include <QObject>


// Nodes that interact with custom view widgets and/or get info
// sent to them from the engine (and this does both) need to inherit from both
//   "Node" (so they can be used by Cues)
//   "QObject" (so they can have signals & slots)
#if 0
class SublevelAdaptor : public QObject
{
    Q_OBJECT

public:
    SublevelAdaptor();


public slots:
    void spectrumChanged(const FrequencySpectrum &spectrum);

signals:
    // used by SublevelMeter widget:
    void levelChanged(qreal level);
    // used by Spectrograph widget:
    void showSubrange(const Subrange &subrange);

};
#endif

class SublevelNode : public QObject, public Node
{
    Q_OBJECT

public:
    SublevelNode(QObject *parent = 0);
    void calculateLevel();


public slots:
    void spectrumChanged(const FrequencySpectrum &spectrum);

signals:
    // used by SublevelMeter widget:
    void levelChanged(qreal level);
    // used by Spectrograph widget:
    void showSubrange(const Subrange &subrange);

private:
    // Calculated by engine
    FrequencySpectrum   _spectrum;

    // Height of level bar.
    float       _level;

    Subrange    _range;
};

#endif // SUBLEVELNODE_H
