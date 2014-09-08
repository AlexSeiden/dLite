#ifndef SUBLEVEL_H
#define SUBLEVEL_H

#include "Subrange.h"
#include <QTime>
#include <QWidget>

// -----------------------------------------------------------------------------
// Sublevel meter
/*
 * Widget which displays a vertical audio level meter, indicating the
 * RMS level of a subset of audio samples most recently analyzed
 * by the Engine. This subset is those within the specfied frequency
 * range and amplitude.
 * Also has a pulsing display at the top, proportional to rms level
 */
class SublevelMeter : public QWidget
{
    Q_OBJECT

public:
    explicit SublevelMeter(QWidget *parent = 0);
    ~SublevelMeter() { }

    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void setSelectable(bool status);
    bool setSelection(bool status);

    void setActive(bool status) { _active = status; }
    Subrange getRange() {return _range;}
    void setRange(Subrange &newrange) { _range = newrange; }


signals:
    void iveBeenSelected(SublevelMeter *me);

public slots:
    void levelChanged(qreal level);

private:
    // Height of level bar.
    float  _level;

    bool _active;
    bool _selectable;
    bool _selected;
    bool _dragTarget;

protected:
    Subrange    _range;
};

#endif // SUBLEVEL_H
