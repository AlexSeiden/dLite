#include "CuesheetView.h"
#include "GuiSettings.h"
#include <QtWidgets>

#ifndef QT_NO_WHEELEVENT
void GraphicsView::wheelEvent(QWheelEvent *e)
{
    // This is only here to help implement zooming with wheel events.
    // LATER pinch-to-zoom work.
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0)
            view->zoomIn(6);
        else
            view->zoomOut(6);
        e->accept();
    } else {
        QGraphicsView::wheelEvent(e);
    }
}
#endif

CuesheetView::CuesheetView(QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(NoFrame);
    graphicsView = new GraphicsView(this);
    graphicsView->setRenderHint(QPainter::Antialiasing, true);

    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setInteractive(true);

    // ??? Not sure about these flags...they came from the example stuff
    graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

//    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QSize iconSize(GuiSettings::iconSize, GuiSettings::iconSize);

    // Create zoom slider
    QToolButton *zoomInIcon = new QToolButton;
    zoomInIcon->setAutoRepeat(true);
    zoomInIcon->setAutoRepeatInterval(33);
    zoomInIcon->setAutoRepeatDelay(0);
    zoomInIcon->setIcon(QPixmap(":/images/zoomin.png"));
    zoomInIcon->setIconSize(iconSize);
    QToolButton *zoomOutIcon = new QToolButton;
    zoomOutIcon->setAutoRepeat(true);
    zoomOutIcon->setAutoRepeatInterval(33);
    zoomOutIcon->setAutoRepeatDelay(0);
    zoomOutIcon->setIcon(QPixmap(":/images/zoomout.png"));
    zoomOutIcon->setIconSize(iconSize);
    zoomSlider = new QSlider;
    zoomSlider->setMinimum(0);
    zoomSlider->setMaximum(500);
    zoomSlider->setValue(500);
    zoomSlider->setTickPosition(QSlider::TicksRight);

    resetButton = new QToolButton;
    resetButton->setText(tr("0"));
    resetButton->setEnabled(true);

    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout;
    zoomSliderLayout->addWidget(zoomInIcon);
    zoomSliderLayout->addWidget(zoomSlider);
    zoomSliderLayout->addWidget(zoomOutIcon);
    zoomSliderLayout->addWidget(resetButton);

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(graphicsView, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    setLayout(topLayout);

    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetView()));
    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));

    setupMatrix();
}

QGraphicsView *CuesheetView::view() const
{
    // This is only here to help implement zooming with wheel events.
    // LATER make pinch-to-zoom work.
    return static_cast<QGraphicsView *>(graphicsView);
}

void CuesheetView::resetView()
{
    zoomSlider->setValue(500);
    setupMatrix();
    // TODO this doesn't work so great.
    graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
}

void CuesheetView::setupMatrix()
{
    qreal scale = qPow(qreal(2), (zoomSlider->value() - 500) / qreal(200));

    /*
    125-250 = -125
    -125/50 = -2.5
    -500/x = -2.5
    x= 500/2.5 = 20
    */
    QMatrix matrix;
    matrix.scale(scale, scale);

    graphicsView->setMatrix(matrix);
}

void CuesheetView::zoomIn(int level)
{
    zoomSlider->setValue(zoomSlider->value() + level);
}

void CuesheetView::zoomOut(int level)
{
    zoomSlider->setValue(zoomSlider->value() - level);
}
