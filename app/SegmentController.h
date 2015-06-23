#ifndef SEGMENTCONTROLLER_H
#define SEGMENTCONTROLLER_H

#include <QWidget>
#include <QMap>
#include <QSet>
#include <QMetaObject>

class QLineEdit;
class QSpinBox;

// ------------------------------------------------------------------------------
// Song segmentation
//      Encapsulates segmentation data
struct Segment {
    int startTime;
    int duration;
    int segmentIndex;
    QString segmentVariant;
};

class SongSegmentation {
public:
    // Each segment
    QList<Segment>  m_segments;

    // Sorted list of segment indices, with no duplicates.
    QList<int>      m_segmentIndices;
    int             m_nSegments; // size of above, precalced for wikid efficientcy
    int             m_duration;
};

Q_DECLARE_METATYPE(SongSegmentation);

// ------------------------------------------------------------------------------
// Segment Controller
//      Switches between cues when segments change.
class SegmentController : public QObject
{
    Q_OBJECT

public:
    SegmentController();

    void    loadFile();
    void    loadFile(QString filename);
    void    loadFileQM(QString filename);

    SongSegmentation _segmentation;

    int findSegment(int msecs);
    int findSegment();

//    friend class SegGui;
};

#endif // SEGMENTCONTROLLER_H
