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
    setWindowTitle(tr("Control Panel"));

    windowLayout = new QVBoxLayout(this);
    hLayout = new QHBoxLayout;
    controlsPanel = new QWidget(this);
    controlsPanel->setLayout(hLayout);
    windowLayout->addWidget(controlsPanel);

    for (int i=0; i<3; ++i)
        newSpectrumSensor();

    // "add control" button
    m_addSensorButton = new QPushButton(this);
//    m_addSensorIcon = QIcon(":/images/settings.png");
//    m_addSensorButton->setIcon(m_addsensorIcon);
    m_addSensorButton->setText(tr("+sensor"));
    m_addSensorButton->setEnabled(true);
    m_addSensorButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_addSensorButton->setMinimumSize(30,30);
    windowLayout->addWidget(m_addSensorButton);

    m_addCueButton = new QPushButton(this);
    m_addCueButton->setText(tr("+cue"));
    m_addCueButton->setEnabled(true);
    m_addCueButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_addCueButton->setMinimumSize(30,30);
    windowLayout->addWidget(m_addCueButton);

    setLayout(windowLayout);

    CHECKED_CONNECT(m_addSensorButton, SIGNAL(clicked()), this, SLOT(newSpectrumSensor()));
    CHECKED_CONNECT(m_addCueButton, SIGNAL(clicked()), this, SLOT(newCue()));



    // TODO restore from saved & allowed saved layouts
    setMinimumHeight(300);
    move(600,50);
}


void Controlpanel::newSpectrumSensor()
{
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

    // TODO use simpler spectrumChanged
    // Get the spectrum as they are calculated.
    CHECKED_CONNECT(_engine, SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
            slm, SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));


    numMeters++;
    meters.append(slm);
    setMinimumWidth(numMeters*40);
}

void Controlpanel::newCue()
{
    Cue *cue = new Cue(_dfModel);
    CueView *cv = new CueView(cue, NULL);
    cv->show();
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
    for (SublevelMeter *slm : meters)  {
        if (slm != chosen) {
            slm->setSelection(false);
        }
    }

    // XXX testing
    auto provider = chosen->createProviderFunctor();

    // emit signal for others who might care, e.g. spectrograph
    emit(submeterSelectionChanged(chosen));
}
