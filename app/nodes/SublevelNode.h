#ifndef SUBLEVELNODE_H
#define SUBLEVELNODE_H

#include "nodes/Node.h"
#include "engine/subrange.h"
#include "engine/frequencyspectrum.h"
#include <QObject>
#include <QMetaType>


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
    ~SublevelNode();

    void operator() ();
    virtual SublevelNode* clone();
    void beenSelected();
    void beenDeselected();

    Subrange *getSubrange() {return &m_range;}
    virtual void readFromJSONObj(const QJsonObject &json);
    virtual void writeToJSONObj(QJsonObject &json) const;


public slots:
    void spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum);
    void setSubrange(Subrange *range) {m_range = *range;}

signals:
    // used for display, by the RangeMeter widget:
    void levelChanged(qreal level);

    // used for editing, by Spectrograph widget:
    void sublevelSelected(SublevelNode *me);
    void sublevelDeselected(SublevelNode *me);

private:
    void calculateLevel();

    // Calculated by engine
    FrequencySpectrum   m_spectrum;
    Subrange            m_range;
};

#endif // SUBLEVELNODE_H
