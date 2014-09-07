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

    mainLayout = new QHBoxLayout;
    mainLayout->setMargin(2);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(4,4,4,4);

    mainLayout->addWidget(createGroup("Cues", Node::CUE));
    mainLayout->addWidget(createGroup("Ints", Node::INT));
    mainLayout->addWidget(createGroup("Floats", Node::FLOAT));
    mainLayout->addWidget(createGroup("Colors", Node::COLOR));
    mainLayout->addWidget(createGroup("Beats", Node::BEAT));
    setLayout(mainLayout);

    setWindowTitle(tr("Cue Library"));
    setWindowFlags(Qt::Window | Qt::WindowTitleHint |
                   Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    CHECKED_CONNECT(signalMapper, SIGNAL(mapped(QString)),
                    this, SIGNAL(newNodeRequest(QString)));
    // The newNodeRequest signal from here is monitored by mainwidget,
    // who does the bulk of the dispatching.

}

QGroupBox *CueLibView::createGroup(QString typeName, Node::node_t nodeType)
{
    QGroupBox *groupBox = new QGroupBox(typeName);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);

    QStringList allNodenames = NodeFactory::Singleton()->getNodesOfType(nodeType);

    foreach (QString nodename, allNodenames) {
        addButtonToUi(layout, nodename);
    }

    layout->addStretch();
    groupBox->setLayout(layout);
    return groupBox;
}

// Convienence function
void CueLibView::addButtonToUi(QLayout *layout, QString buttonName) {
    QPushButton *butt = new QPushButton(buttonName);
    buttons.push_back(butt);
    layout->addWidget(butt);
    signalMapper->setMapping(butt, buttonName);
    CHECKED_CONNECT(butt, SIGNAL(clicked()), signalMapper, SLOT(map()));
}
