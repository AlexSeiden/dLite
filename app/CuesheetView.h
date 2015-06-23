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
    GraphicsView(CuesheetView *v) : QGraphicsView(), m_view(v) { }

protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *);
#endif

private:
    CuesheetView*   m_view;
};

class CuesheetView : public QFrame
{
    Q_OBJECT
public:
    explicit CuesheetView(QWidget *parent = 0);

    QGraphicsView *view() const;

public slots:
    void zoomIn(int level = 10);
    void zoomOut(int level = 10);
    void zoomReset();
    void fitBbox(const QRectF& bbox);

private slots:
    void setSliderFromTransform();
    void resetView();
    void setupMatrix();

signals:
    void newCuesheet();

private:
    GraphicsView*   m_graphicsView;
    QToolButton*    m_resetButton;
    QToolButton*    m_newcueButton;
    QSlider*        m_zoomSlider;
};

#endif // CUESHEETVIEW_H
