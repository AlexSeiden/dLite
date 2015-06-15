#include "CuesheetView.h"
#include "GuiSettings.h"
#include <QtWidgets>
#include "utils.h"

CuesheetView::CuesheetView(QWidget *parent)
    : QFrame(parent)
{
    setContentsMargins(2,1,2,1);
    setFrameStyle(NoFrame);
    _graphicsView = new GraphicsView(this);
    _graphicsView->setRenderHint(QPainter::Antialiasing, true);

    _graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    _graphicsView->setInteractive(true);

//    setStyleSheet("QFrame { background-color: #433F3B }");

    // ??? Not sure about these flags...they came from the example stuff
    _graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    _graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    _graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

//    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QSize iconSize(guisettings->zoomIconSize, guisettings->zoomIconSize);

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
    _zoomSlider = new QSlider;
    _zoomSlider->setMinimum(0);
    _zoomSlider->setMaximum(700);
    _zoomSlider->setValue(700);
    _zoomSlider->setTickPosition(QSlider::TicksRight);

    _resetButton = new QToolButton;
    _resetButton->setText(tr("0"));
    _resetButton->setEnabled(true);

//    _newcueButton = new QToolButton;
//    _newcueButton->setText(tr("+"));
//    _newcueButton->setEnabled(true);

    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout;
    zoomSliderLayout->addWidget(zoomInIcon);
    zoomSliderLayout->addWidget(_zoomSlider);
    zoomSliderLayout->addWidget(zoomOutIcon);
    zoomSliderLayout->addWidget(_resetButton);
    zoomSliderLayout->setAlignment(_zoomSlider, Qt::AlignRight);
//    zoomSliderLayout->addWidget(_newcueButton);

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(_graphicsView, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    topLayout->setContentsMargins(4,4,2,4);
    setLayout(topLayout);

//    connect(_newcueButton, SIGNAL(clicked()), this, SIGNAL(newCuesheet()));
    connect(_resetButton, SIGNAL(clicked()), this, SLOT(resetView()));
    connect(_zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));

    setupMatrix();
}

QGraphicsView *CuesheetView::view() const
{
    // This is only here to help implement zooming with wheel events.
    // LATER make pinch-to-zoom work.
    return static_cast<QGraphicsView *>(_graphicsView);
}

void CuesheetView::resetView()
{
    _zoomSlider->setValue(500);
    setupMatrix();
    // TODO this doesn't work so great.
    _graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
}

void CuesheetView::fitBbox(const QRectF &bbox)
{
//    view()->fitInView(bbox, Qt::KeepAspectRatioByExpanding);
    view()->fitInView(bbox, Qt::KeepAspectRatio);
    setSliderFromTransform();
}

void CuesheetView::setSliderFromTransform()
{
    // Find scale from xform matrix:
    QTransform xform = _graphicsView->transform();
    qreal scale = xform.m11();
    // m22 should be the same... could check....

    // Compute the slider value from the xform matrix that exists:
    int slider = 200 * log2(scale) + 700;

//    qDebug() << scale << slider;
    _zoomSlider->setValue(slider);
    return;


    // Set the slider value, but not so much that it zooms in beyond "700",
    // which is overkill.
    if (slider<0 || slider>700) {
        slider = clamp(0,700,slider);
        _zoomSlider->setValue(slider);
        setupMatrix();
    }
    else
        _zoomSlider->setValue(slider);
}

void CuesheetView::setupMatrix()
{
    qreal scale = qPow(2., (_zoomSlider->value() - 700.) / 200.);
    _graphicsView->setTransform(QTransform());
    _graphicsView->scale(scale, scale);
//    qDebug() << "setting scale" << scale << "from slider" << _zoomSlider->value();
}

void CuesheetView::zoomIn(int level)
{
    _zoomSlider->setValue(_zoomSlider->value() + level);
}

void CuesheetView::zoomOut(int level)
{
    _zoomSlider->setValue(_zoomSlider->value() - level);
}

void CuesheetView::zoomReset()
{
    _zoomSlider->setValue(700);
}

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
