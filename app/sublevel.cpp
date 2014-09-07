#include "sublevel.h"
#include "GuiColors.h"
#include "utils.h"
#include "Param.h"
#include "CueView.h"

#include <math.h>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>


// -----------------------------------------------------------------------------
// Subrange

Subrange::Subrange() :
    freqMin(100.0),
    freqMax(200.0),
    ampMin(0.0),
    ampMax(1.0)
{ }

Subrange::~Subrange() { }

/*
 * Returns 0.0 if the given amplitude is below the minimum
 * threshold for the subrange, 1.0 if it's above it, and
 * a proportional value in between.
 */
double Subrange::amplitudeWithinWindow(double amp)
{
    if (amp < ampMin)
        return 0.0;
    if (amp > ampMax)
        return 1.0;
    return ((amp-ampMin)/(ampMax-ampMin));
}

/*
 * Returns true if a given frequency is within the window
 * set for this range.
 */
bool Subrange::isFrequencyWithinWindow(double freq)
{
    if (freq < freqMin)
        return false;
    if (freq > freqMax)
        return false;
    return true;
}


// -----------------------------------------------------------------------------
// SublevelMeter

SublevelMeter::SublevelMeter(QWidget *parent)
    :   QWidget(parent)
    ,   m_level(0.0)
    ,   _active(false)
    ,   _selectable(false)
    ,   _selected(false)
    ,   _dragTarget(false)
    ,   _cue(NULL)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setMinimumWidth(30);
    setAcceptDrops(true);
}

SublevelMeter::~SublevelMeter() {  }

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
    _selected = status;
    update();
    return true;
}

void SublevelMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), GuiSettings::sl_bgColor);

    // Draw bar
    QRect bar = rect();
    bar.setTop(rect().top() + (1.0 - m_level) * rect().height());
    painter.fillRect(bar, GuiSettings::sl_barColor);

    // Draw pulsar
    QRect squareRect = rect();
    squareRect.setBottom(rect().top()+ rect().width());
    squareRect.setRight(rect().right()-1);
    QColor pulseColor;
    pulseColor.setHsvF(0.0, 0.0, m_level);
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

    auto closure = createProviderClosure();
    paramView->setProvider(closure);

    _dragTarget = false;

    // select this meter
    this->setSelection(true);
    emit(iveBeenSelected(this));

    update();
}


// TODO this is business logic that shouldn't be in a view class.
// TODO decouple position & spectrum change
void SublevelMeter::spectrumChanged(qint64 position, qint64 length,
                                 const FrequencySpectrum &spectrum)
{
    Q_UNUSED(position);
    Q_UNUSED(length);
    m_spectrum = spectrum;
    if (_active) {
        calculateLevel();
        emit levelChanged(m_level);
        update();
    }
}

// TODO this is business logic that shouldn't be in a view class.
void SublevelMeter::calculateLevel()
{
    // loop over all frequencies in the spectrum, and set the value
    FrequencySpectrum::const_iterator i = m_spectrum.begin();
    const FrequencySpectrum::const_iterator end = m_spectrum.end();
    int index = 0;

    float value = 0;
    int nsamples = 0;
    bool clipped = false;

    for ( ; i != end; ++i, ++index) {
        const FrequencySpectrum::Element e = *i;

        // TODO could optimize by skipping straight to start frequency
        if (_range.isFrequencyWithinWindow(e.frequency)) {
            // amplitude window
            value += _range.amplitudeWithinWindow(e.amplitude);
            nsamples++;
            clipped |= e.clipped;
        }
    }

    // Compute subrange amplitude
    if (nsamples > 0)
        value /= nsamples;
    if (value > 1.0) {
        value = 1.0;
        clipped = true;
    }

    m_level = value;
    update();

    //emit subrangeLevelChanged(value);
}

// TODO this is business logic that shouldn't be in a view class.
//template<class PT> std::function<void(PT&)> SublevelMeter::createProviderClosure()
// TODO could use a functor instead.
std::function<void(float&)> SublevelMeter::createProviderClosure()
{
    return [this] (float &out) {out = m_level;};
}

