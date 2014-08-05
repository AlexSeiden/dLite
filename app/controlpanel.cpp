#include "controlpanel.h"
#include "levelmeter.h"
#include "sublevel.h"
#include "utils.h"
#include <QHBoxLayout>
#include <QMouseEvent>


Controlpanel::Controlpanel(QWidget *parent, Engine *engine, Dancefloormodel *dfmodel) :
    QWidget(parent),
    numMeters(0),
    _engine(engine),
    _dfmodel(dfmodel),
    _cue(NULL)
{
    _cue = new Cue(NULL, _dfmodel);
    createUi();
}

void Controlpanel::createUi()
{
    setWindowTitle(tr("Control Panel"));
    hLayout = new QHBoxLayout(this);

    for (int i=0; i<3; ++i)
        addMeter();

    CHECKED_CONNECT(meters[0], SIGNAL(levelChanged(qreal)), _cue, SLOT(levelChanged(qreal)));

    setLayout(hLayout);

    // TODO "add control" button

    // TODO restore from saved & allowed saved layouts
    setMinimumHeight(300);
    move(600,50);
}


void Controlpanel::addMeter()
{
    SublevelMeter *slm = new SublevelMeter(this);
    slm->setSelectable(true);
    hLayout->addWidget(slm);
    CHECKED_CONNECT(slm, SIGNAL(iveBeenSelected(SublevelMeter*)),
            this, SLOT(submeterHasBeenSelected(SublevelMeter*)));

    // TODO use simpler spectrumChanged
    CHECKED_CONNECT(_engine, SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
            slm, SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));

    numMeters++;
    meters.append(slm);
    setMinimumWidth(numMeters*40);
}

void Controlpanel::mouseReleaseEvent(QMouseEvent *event)
{
    // TODO Deselect, since it means that we've clicked outside of all the
    // controller windows
    event->ignore();
}

void Controlpanel::submeterHasBeenSelected(SublevelMeter *chosen)
{
    // Unselect others:
    // for (SublevelMeter *slm : meters) // C++11 extension.
    for (int i=0; i<meters.size(); ++i)
    {
        SublevelMeter *slm = meters[i];

        if (slm != chosen) {
            slm->setSelection(false);
        }
    }

    // emit signal for others who might care, e.g. spectrograph
    emit(submeterSelectionChanged(chosen));
}
