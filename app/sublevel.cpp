#include "sublevel.h"
#include "GuiColors.h"
#include "utils.h"
#include "Param.h"
#include "CueView.h"

#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>


// -----------------------------------------------------------------------------
// SublevelMeter

SublevelMeter::SublevelMeter(QWidget *parent)
    :   QWidget(parent)
    ,   _level(0.0)
    ,   _active(false)
    ,   _selectable(false)
    ,   _selected(false)
    ,   _dragTarget(false)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(30,100); // GUI
    setAcceptDrops(true);
}

void SublevelMeter::setSelectable(bool status)
{
    _selectable = status;
    if (!_selectable)
        _selected = false;
}

bool SublevelMeter::setSelection(bool status)
{
    if (!_selectable) {
        return false;
    }

    if (_selected == status)
        return true;

    _selected = status;
    update();

    // emit signal so that other meters can deselect, and the spectrograph
    // can draw the appropriate window.
    // XXX possible endless loop of signals?
    if (_selected) {
        emit iveBeenSelected(this);
        // Kinda GROSS ...handled in a bunch of places.
        // TODO emit drawThisSubrange(this->_subrange);
    }
    else
        emit iveBeenSelected(NULL);

    return true;
}

void SublevelMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), GuiSettings::sl_bgColor);

    // Draw bar
    QRect bar = rect();
    bar.setTop(rect().top() + (1.0 - _level) * rect().height());
    painter.fillRect(bar, GuiSettings::sl_barColor);

    // Draw pulsar
    QRect squareRect = rect();
    squareRect.setBottom(rect().top()+ rect().width());
    squareRect.setRight(rect().right()-1);
    QColor pulseColor;
    pulseColor.setHsvF(0.0, 0.0, _level);
    painter.fillRect(squareRect, pulseColor);
    painter.drawRect(squareRect);

    if (_selected) {
        QPen pen(GuiSettings::sl_selectedColor);
        pen.setWidth(6);
        painter.setPen(pen);

        QRect surroundRect = rect();
        // move in 1 pixel
        surroundRect.setSize(QSize(rect().width()-1, rect().height()-1));
        painter.drawRect(rect());
    }

    if (_dragTarget) {
        QPen pen(GuiSettings::sl_dragTarget);
        pen.setWidth(6);
        painter.setPen(pen);

        QRect surroundRect = rect();
        // move in 1 pixel
        surroundRect.setSize(QSize(rect().width()-1, rect().height()-1));
        painter.drawRect(rect());
    }
}

// Mouse release events handle selection
void SublevelMeter::mouseReleaseEvent(QMouseEvent *event)
{
    if (!_selectable) {
        event->ignore();
        return;
    }

    //If already selected, a second click deselects:
    if (_selected) {
        setSelection(false);
        emit(iveBeenSelected(NULL));
    }
    else {
        setSelection(true);
        emit(iveBeenSelected(this));
    }
}

// Drag & drop for whip connect
void SublevelMeter::dragEnterEvent(QDragEnterEvent *event)
{
    if (! event->mimeData()->hasFormat(paramTypeFloat.name()))
        return;

    event->acceptProposedAction();
    _dragTarget = true;
    update();

}

void SublevelMeter::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    _dragTarget = false;
    update();
}

void SublevelMeter::dropEvent(QDropEvent *event)
{
    if (! event->mimeData()->hasFormat(paramTypeFloat.name()))
        return;

    event->acceptProposedAction();

    // TODO emit signal that drop is accepted
    // create closure for value, and return it.
    // or is it better to just pass the param handle through mime?

    QObject *eventSrc = event->source();
    ParamView *paramView = qobject_cast<ParamView *>(eventSrc->parent());
    Q_ASSERT(paramView);

#if 0
    // XXX this is troublesome here.
    auto closure = createProviderClosure();
    // the view receives this action, but needs the model to change.
    // need something like
    paramView->getParam()->connectTo(this->getParam());
    // or
    emit outputSeeksInput(this->getParam())
#endif



    _dragTarget = false;
    // select this meter
    this->setSelection(true);

    update();
}

void SublevelMeter::levelChanged(qreal level)
{
    _level = level;
    update();
}
