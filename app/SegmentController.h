#ifndef SEGMENTCONTROLLER_H
#define SEGMENTCONTROLLER_H

#include <QWidget>
#include <QMap>
#include <QSet>
#include <QMetaObject>

class QLineEdit;
class QSpinBox;

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
};

Q_DECLARE_METATYPE(SongSegmentation);

class SegmentController : QObject
{
    Q_OBJECT

public:
    SegmentController();

    void    loadFile();
    void    loadFile(QString filename);

    SongSegmentation _segmentation;
private:
//    std::vector<std::pair<int, int> > _segments;


    int findSegment(int msecs);
    int findSegment();

    friend class SegGui;
};

class SegGui : public QWidget
{
    Q_OBJECT
public:
    SegGui(SegmentController *segcont, QWidget *parent = 0);

public slots:
    int whatToActivate();

signals:
    void setCuesheet(int);

private:
    SegmentController *_sc;
    QMap<int, QLineEdit*> _indexToLE;
    QMap<int, QSpinBox*> _indexToSpinbox;


};

#endif // SEGMENTCONTROLLER_H
