#ifndef CUESHEET_H
#define CUESHEET_H

#include <QFrame>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QToolButton;
QT_END_NAMESPACE

class Cuesheet;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(Cuesheet *v) : QGraphicsView(), view(v) { }

protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *);
#endif

private:
    Cuesheet *view;
};

class Cuesheet : public QFrame
{
    Q_OBJECT
public:
    explicit Cuesheet(QWidget *parent = 0);

    QGraphicsView *view() const;

public slots:
    void zoomIn(int level = 1);
    void zoomOut(int level = 1);

private slots:
    void resetView();
    void setupMatrix();

private:
    GraphicsView *graphicsView;
    QToolButton *resetButton;
    QSlider *zoomSlider;
};

#endif // CUESHEET_H
