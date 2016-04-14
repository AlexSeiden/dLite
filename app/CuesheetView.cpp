#include "CuesheetView.h"
#include "GuiSettings.h"
#include <QtWidgets>
#include "utils.h"

CuesheetView::CuesheetView(QWidget *parent)
    : QFrame(parent)
{
    setContentsMargins(2,1,2,1);
    setFrameStyle(NoFrame);
    m_graphicsView = new GraphicsView(this);
    m_graphicsView->setRenderHint(QPainter::Antialiasing, true);

    m_graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    m_graphicsView->setInteractive(true);

    m_graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    m_graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_graphicsView);
    setLayout(layout);

    zoomReset();
    setupMatrix();
}

QGraphicsView *CuesheetView::view() const
{
    // This is only here to help implement zooming with wheel events.
    // TODO make pinch-to-zoom work.
    return static_cast<QGraphicsView *>(m_graphicsView);
}

void CuesheetView::resetView()
{
    m_zoom = 500;
    setupMatrix();
    m_graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
}

void CuesheetView::fitBbox(const QRectF &bbox)
{
    view()->fitInView(bbox, Qt::KeepAspectRatio);
    setZoomFromTransform();
}

void CuesheetView::setZoomFromTransform()
{
    // Find scale from xform matrix:
    QTransform xform = m_graphicsView->transform();
    qreal scale = xform.m11();
    // m22 should be the same as m11, since we don't want to scale anamorphically!

    // Compute the zoom value from the xform matrix that exists:
    m_zoom = 200 * log2(scale) + 700;
    m_zoom = clamp(0., 700., m_zoom);
    setupMatrix();
}

void CuesheetView::setupMatrix()
{
    qreal scale = qPow(2., (m_zoom - 700.) / 200.);
    m_graphicsView->setTransform(QTransform());
    m_graphicsView->scale(scale, scale);
}

void CuesheetView::zoomIn(int level)
{
    m_zoom += level;
    m_zoom = clamp(0., 1200., m_zoom);
    setupMatrix();
}

void CuesheetView::zoomOut(int level)
{
    m_zoom -= level;
    m_zoom = clamp(0., 1200., m_zoom);
    setupMatrix();
}

void CuesheetView::zoomReset()
{
    m_zoom = 700;
    setupMatrix();
}

#ifndef QT_NO_WHEELEVENT
void GraphicsView::wheelEvent(QWheelEvent *e)
{
    // This is only here to help implement zooming with wheel events.
    // TODO make pinch-to-zoom work.
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0)
            m_view->zoomIn(6);
        else
            m_view->zoomOut(6);
        e->accept();
    } else {
        QGraphicsView::wheelEvent(e);
    }
}
#endif
