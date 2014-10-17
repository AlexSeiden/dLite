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
    QList<Segment>  _segments;
    // Sorted list of segment indices, with no duplicates.
    QList<int>      _segmentIndices;
    int             _duration;
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

    SongSegmentation _segmentation;

    int findSegment(int msecs);
    int findSegment();

//    friend class SegGui;
};

#if 0
// ------------------------------------------------------------------------------
// SegGui
//      Widget that displays segment controller.
class SegGui : public QWidget
{
    Q_OBJECT
public:
    SegGui(SegmentController *segcont, QWidget *parent = 0);

public slots:
    int whatToActivate();
    void setNumCues(int nCues);

signals:
    void setCuesheet(int);

public:  // GROSS !!
    SegmentController*      _sc;
    QMap<int, QLineEdit*>   _indexToLE;
    QMap<int, QSpinBox*>    _indexToSpinbox;
    int                     _nCues;
};
#endif
#endif // SEGMENTCONTROLLER_H
