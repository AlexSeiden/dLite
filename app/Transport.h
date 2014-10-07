#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QWidget>
#include <QIcon>
#include "SegmentController.h"

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class Transport : public QWidget
{
    Q_OBJECT
public:
    explicit Transport(QWidget *parent = 0);
    ~Transport();

    void reset();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

public slots:
    void bufferLengthChanged(qint64 length);
    void playPositionChanged(qint64 playPosition);
    void segmentsChanged(SongSegmentation*);

signals:
    void movePlaybackHead(double pos);

private:
    qint64 m_bufferLength;
    qint64 m_playPosition;

    QPushButton*            _playPauseButton;
    QIcon                   _pauseIcon;
    QIcon                   _playIcon;
    SongSegmentation*       _segmentation;
    int                     _totalDuration;
};

#endif // TRANSPORT_H
