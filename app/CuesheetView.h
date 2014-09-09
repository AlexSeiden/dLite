#ifndef CUESHEETVIEW_H
#define CUESHEETVIEW_H

// The graph display widget, where Nodes & Cues are viewed &
// connected.

#include <QFrame>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QToolButton;
QT_END_NAMESPACE

class CuesheetView;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(CuesheetView *v) : QGraphicsView(), view(v) { }

protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *);
#endif

private:
    CuesheetView *view;
};

class CuesheetView : public QFrame
{
    Q_OBJECT
public:
    explicit CuesheetView(QWidget *parent = 0);

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

#endif // CUESHEETVIEW_H