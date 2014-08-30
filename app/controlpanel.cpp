#include "controlpanel.h"
#include "levelmeter.h"
#include "sublevel.h"
#include "utils.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QLabel>


Controlpanel::Controlpanel(QWidget *parent,
                           Engine *engine,
                           Dancefloormodel *dfmodel) :
    QWidget(parent),
    numMeters(0),
    _engine(engine),
    _dfModel(dfmodel)
{
    createUi();
}

void Controlpanel::createUi()
{
    setWindowFlags(Qt::Tool | Qt::WindowTitleHint  |
                   Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setWindowTitle(tr("Control Panel"));

    windowLayout = new QVBoxLayout(this);
    windowLayout->setSpacing(0);
    windowLayout->setContentsMargins(0,0,0,0);

    hLayout = new QHBoxLayout;
    controlsPanel = new QWidget(this);
    controlsPanel->setLayout(hLayout);
    windowLayout->addWidget(controlsPanel);

    for (int i=0; i<3; ++i)  // XXX arbitrary, for debugging
        newSpectrumSensor();

    setLayout(windowLayout);

    // TODO restore from saved & allowed saved layouts
    setMinimumHeight(300);
    move(600,50);
}

void Controlpanel::newSpectrumSensor()
{
    // TODO disentangle the spectrum view logic from the "business" logic
    SublevelMeter *slm = new SublevelMeter(this);
    slm->setSelectable(true);

    QLabel *meterNumber = new QLabel(QString("%1").arg(numMeters), this);
    meterNumber->setFixedHeight(20);
    meterNumber->setAlignment(Qt::AlignBottom | Qt::AlignCenter);

    // Make VBox for label, checkbox etc
    QScopedPointer<QVBoxLayout> vbox(new QVBoxLayout); // ??? why is this scoped pointer stuff needed
    vbox->addWidget(slm);
    vbox->addWidget(meterNumber);

    hLayout->addLayout(vbox.data());
    vbox.take();    // ownership transferred to hLayout

    // Connect submeter so that when it's selected, the spectrograph
    // displays its window.
    CHECKED_CONNECT(slm, SIGNAL(iveBeenSelected(SublevelMeter*)),
            this, SLOT(submeterHasBeenSelected(SublevelMeter*)));

    // Get the spectrum as they are calculated.
    // TODO use simpler spectrumChanged
    CHECKED_CONNECT(_engine, SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
            slm, SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));

    numMeters++;
    meters.append(slm);
    setMinimumWidth(numMeters*40);
}

void Controlpanel::submeterHasBeenSelected(SublevelMeter *chosen)
{
    // Unselect others:
    for (SublevelMeter *slm : meters)  {
        if (slm != chosen) {
            slm->setSelection(false);
        }
    }

    // XXX testing
    //auto provider = chosen->createProviderClosure();

    // emit signal for others who might care, e.g. spectrograph
    emit(submeterSelectionChanged(chosen));
}

void Controlpanel::mouseReleaseEvent(QMouseEvent *event)
{
    // TODO Deselect, since it means that we've clicked outside of all the
    // controller windows
    event->ignore();
}
