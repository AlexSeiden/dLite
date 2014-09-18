#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QWidget>
#include <QIcon>

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
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void bufferLengthChanged(qint64 length);
    void playPositionChanged(qint64 playPosition);

private:
    qint64 m_bufferLength;
    qint64 m_playPosition;

    QPushButton*            _playPauseButton;
    QIcon                   _pauseIcon;
    QIcon                   _playIcon;
};

#endif // TRANSPORT_H
