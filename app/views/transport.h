// Copyright (c) 2014-2016 Alex Seiden
//
#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QWidget>
#include "engine/segmentation.h"

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
    qint64                  m_bufferLength;
    qint64                  m_playPosition;
    SongSegmentation*       m_segmentation;
    int                     m_totalDuration;
};

#endif // TRANSPORT_H
