#include <QtWidgets>

#include "CueLibView.h"
#include "utils.h"

CueLibView::CueLibView(QWidget *parent) :
    QWidget(parent)
{
    createUi();
    move(500, 50);
}

void CueLibView::createUi()
{
    signalMapper = new QSignalMapper(this);

    // TODO generate these from a single place, where all cues are listed.
    createCuesGroup();
    createFloatsGroup();
    createColorsGroup();
    createBeatsGroup();

    mainLayout = new QHBoxLayout;
    mainLayout->setMargin(2);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(4,4,4,4);

    mainLayout->addWidget(cuesGroupBox);
    mainLayout->addWidget(floatsGroupBox);
    mainLayout->addWidget(colorsGroupBox);
    mainLayout->addWidget(beatsGroupBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Cue Library"));
    setWindowFlags(Qt::Window | Qt::WindowTitleHint |
                   Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    CHECKED_CONNECT(signalMapper, SIGNAL(mapped(QString)),
                    this, SIGNAL(newNodeRequest(QString)));
    // The newNodeRequest signal from here is monitored by mainwidget,
    // who does the bulk of the dispatching.

}

void CueLibView::createCuesGroup()
{
    cuesGroupBox = new QGroupBox(tr("Cues"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);

    addButtonToUi(layout, tr("Box cue"));

    layout->addStretch();
    cuesGroupBox->setLayout(layout);
}

void CueLibView::createFloatsGroup()
{
    floatsGroupBox = new QGroupBox(tr("Floats"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);

    QStringList allFloats;
    allFloats = NodeRegistry.getNodesOfType(Node::FLOAT);


    foreach (QString nodename, allFloats) {
        addButtonToUi(layout, nodename);
    }

//    addButtonToUi(layout, tr("Spectrum range"));
//    addButtonToUi(layout, tr("Random"));

    layout->addStretch();
    floatsGroupBox->setLayout(layout);
}

void CueLibView::createColorsGroup()
{
    colorsGroupBox = new QGroupBox(tr("Colors"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);

    addButtonToUi(layout, tr("Spline"));
    addButtonToUi(layout, tr("Palette"));

    layout->addStretch();
    colorsGroupBox->setLayout(layout);
}

void CueLibView::createBeatsGroup()
{
    beatsGroupBox = new QGroupBox(tr("Beats"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);

    addButtonToUi(layout, tr("Bar"));
    addButtonToUi(layout, tr("Beat"));
    addButtonToUi(layout, tr("Onset"));

    layout->addStretch();
    beatsGroupBox->setLayout(layout);
}

// Convienence function
void CueLibView::addButtonToUi(QLayout *layout, QString buttonName) {
    QPushButton *butt = new QPushButton(buttonName);
    buttons.push_back(butt);
    layout->addWidget(butt);
    signalMapper->setMapping(butt, buttonName);
    CHECKED_CONNECT(butt, SIGNAL(clicked()), signalMapper, SLOT(map()));
}
