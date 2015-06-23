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

    // ??? Not sure about these flags...they came from the example stuff
    m_graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    m_graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

//    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QSize iconSize(guisettings->m_zoomIconSize, guisettings->m_zoomIconSize);

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
    m_zoomSlider = new QSlider;
    m_zoomSlider->setMinimum(0);
    m_zoomSlider->setMaximum(700);
    m_zoomSlider->setValue(700);
    m_zoomSlider->setTickPosition(QSlider::TicksRight);

    m_resetButton = new QToolButton;
    m_resetButton->setText(tr("0"));
    m_resetButton->setEnabled(true);

//    _newcueButton = new QToolButton;
//    _newcueButton->setText(tr("+"));
//    _newcueButton->setEnabled(true);

    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout;
    zoomSliderLayout->addWidget(zoomInIcon);
    zoomSliderLayout->addWidget(m_zoomSlider);
    zoomSliderLayout->addWidget(zoomOutIcon);
    zoomSliderLayout->addWidget(m_resetButton);
    zoomSliderLayout->setAlignment(m_zoomSlider, Qt::AlignRight);
//    zoomSliderLayout->addWidget(_newcueButton);

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(m_graphicsView, 1, 0);
    topLayout->addLayout(zoomSliderLayout, 1, 1);
    topLayout->setContentsMargins(4,4,2,4);
    setLayout(topLayout);

//    connect(_newcueButton, SIGNAL(clicked()), this, SIGNAL(newCuesheet()));
    connect(m_resetButton, SIGNAL(clicked()), this, SLOT(resetView()));
    connect(m_zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));

    setupMatrix();
}

QGraphicsView *CuesheetView::view() const
{
    // This is only here to help implement zooming with wheel events.
    // LATER make pinch-to-zoom work.
    return static_cast<QGraphicsView *>(m_graphicsView);
}

void CuesheetView::resetView()
{
    m_zoomSlider->setValue(500);
    setupMatrix();
    // TODO this doesn't work so great.
    m_graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
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
    QTransform xform = m_graphicsView->transform();
    qreal scale = xform.m11();
    // m22 should be the same... could check....

    // Compute the slider value from the xform matrix that exists:
    int slider = 200 * log2(scale) + 700;

    m_zoomSlider->setValue(slider);
    return;

    // Set the slider value, but not so much that it zooms in beyond "700",
    // which is overkill.
    if (slider<0 || slider>700) {
        slider = clamp(0,700,slider);
        m_zoomSlider->setValue(slider);
        setupMatrix();
    }
    else
        m_zoomSlider->setValue(slider);
}

void CuesheetView::setupMatrix()
{
    qreal scale = qPow(2., (m_zoomSlider->value() - 700.) / 200.);
    m_graphicsView->setTransform(QTransform());
    m_graphicsView->scale(scale, scale);
}

void CuesheetView::zoomIn(int level)
{
    m_zoomSlider->setValue(m_zoomSlider->value() + level);
}

void CuesheetView::zoomOut(int level)
{
    m_zoomSlider->setValue(m_zoomSlider->value() - level);
}

void CuesheetView::zoomReset()
{
    m_zoomSlider->setValue(700);
}

#ifndef QT_NO_WHEELEVENT
void GraphicsView::wheelEvent(QWheelEvent *e)
{
    // This is only here to help implement zooming with wheel events.
    // LATER pinch-to-zoom work.
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
