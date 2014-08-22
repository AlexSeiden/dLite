#include "controlpanel.h"
#include "levelmeter.h"
#include "sublevel.h"
#include "utils.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QLabel>


Controlpanel::Controlpanel(QWidget *parent, Engine *engine, Dancefloormodel *dfmodel) :
    QWidget(parent),
    numMeters(0),
    _engine(engine),
    _dfmodel(dfmodel),
    _cue(NULL)
{
    _cue = new Cue(_dfmodel);
    _dfmodel->addCue(_cue); // TODO make this part of cue constructor???

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
        addMeter();
    //CHECKED_CONNECT(meters[0], SIGNAL(levelChanged(qreal)), _cue, SLOT(levelChanged(qreal)));

    // "add control" button
    m_addsensorButton = new QPushButton(this);
//    m_addsensorIcon = QIcon(":/images/settings.png");
//    m_addsensorButton->setIcon(m_addsensorIcon);
    m_addsensorButton->setText(tr("+"));
    m_addsensorButton->setEnabled(true);
    m_addsensorButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_addsensorButton->setMinimumSize(30,30);
    //m_addsensorButton->setLayout(windowLayout);

    windowLayout->addWidget(m_addsensorButton);

    setLayout(windowLayout);

    // TODO restore from saved & allowed saved layouts
    setMinimumHeight(300);
    move(600,50);
}


void Controlpanel::addMeter()
{
    SublevelMeter *slm = new SublevelMeter(this);
    slm->setSelectable(true);

    QLabel *meterNumber = new QLabel(QString("%1").arg(numMeters), this);
    meterNumber->setFixedHeight(20);
    meterNumber->setAlignment(Qt::AlignBottom | Qt::AlignCenter);

    // Make VBox for label, checkbox etc
    QScopedPointer<QVBoxLayout> vbox(new QVBoxLayout);
    vbox->addWidget(slm);
    vbox->addWidget(meterNumber);

    hLayout->addLayout(vbox.data());
    vbox.take();    // ownership transferred to hLayout

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
    for (SublevelMeter *slm : meters)  {
        if (slm != chosen) {
            slm->setSelection(false);
        }
    }

    // XXX testing
    auto provider = chosen->createProviderFunctor();
    _cue->_alpha.setProvider(provider);

    // emit signal for others who might care, e.g. spectrograph
    emit(submeterSelectionChanged(chosen));
}
