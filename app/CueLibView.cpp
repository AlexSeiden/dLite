#include <QtWidgets>

#include "CueLibView.h"
#include "utils.h"

CueLibView::CueLibView(QWidget *parent) :
    QWidget(parent)
{
    createUi();
}

void CueLibView::createUi()
{
    signalMapper = new QSignalMapper(this);

    // TODO generate these from a single place, where all cues are listed.
    createCuesGroup();
    createFloatsGroup();
    createColorsGroup();

    mainLayout = new QHBoxLayout;

    mainLayout->addWidget(cuesGroupBox);
    mainLayout->addWidget(floatsGroupBox);
    mainLayout->addWidget(colorsGroupBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Cue Library"));

    CHECKED_CONNECT(signalMapper, SIGNAL(mapped(QString)),
                    this, SIGNAL(newNodeRequest(QString)));

}

void CueLibView::createCuesGroup()
{
    cuesGroupBox = new QGroupBox(tr("Cues"));
    QVBoxLayout *layout = new QVBoxLayout;

    addButtonToUi(layout, tr("Box cue"));

    layout->addStretch();
    cuesGroupBox->setLayout(layout);
}


void CueLibView::createFloatsGroup()
{
    floatsGroupBox = new QGroupBox(tr("Floats"));
    QVBoxLayout *layout = new QVBoxLayout;

    addButtonToUi(layout, tr("Spectrum range"));
    addButtonToUi(layout, tr("Random"));

    layout->addStretch();
    floatsGroupBox->setLayout(layout);
}


void CueLibView::createColorsGroup()
{
    colorsGroupBox = new QGroupBox(tr("Colors"));
    QVBoxLayout *layout = new QVBoxLayout;

    addButtonToUi(layout, tr("Spline"));
    addButtonToUi(layout, tr("Palette"));

    layout->addStretch();
    colorsGroupBox->setLayout(layout);
}


void
CueLibView::addButtonToUi(QLayout *layout, QString buttonName) {
    QPushButton *butt = new QPushButton(buttonName);
    buttons.push_back(butt);
    layout->addWidget(butt);
    // Note that the newNodeRequsest "Slot" is actually a signal, which will
    // be handled by the mainwidget.
    signalMapper->setMapping(butt, buttonName);
    CHECKED_CONNECT(butt, SIGNAL(clicked()), signalMapper, SLOT(map()));
}
