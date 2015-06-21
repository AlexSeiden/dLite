#include <QtWidgets>

#include "CueLibView.h"
#include "NodeFactory.h"
#include "utils.h"

// ------------------------------------------------------------------------------
// CueLibView
// View that allows the user to create new nodes.

CueLibView::CueLibView(QWidget *parent) :
    QWidget(parent)
{
    createUi();
    move(610, 10);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void CueLibView::createUi()
{
    _signalMapper = new QSignalMapper(this);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setMargin(2);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(2,2,2,2);

    mainLayout->addWidget(createGroup("Cues", Node::CUE),0,0);
    mainLayout->addWidget(createGroup("Ints", Node::INT),0,1);
    mainLayout->addWidget(createGroup("Floats", Node::FLOAT),0,2);
    mainLayout->addWidget(createGroup("Colors", Node::COLOR),1,0);
    mainLayout->addWidget(createGroup("Beats", Node::BEAT),1,1);
    mainLayout->addWidget(createGroup("Paths", Node::POSITION),1,2);
    mainLayout->addWidget(createGroup("Regions", Node::REGION),1,3);
    setLayout(mainLayout);

    setWindowTitle(tr("Cue Library"));
    setWindowFlags( Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    CHECKED_CONNECT(_signalMapper, SIGNAL(mapped(QString)),
                    this, SIGNAL(newNodeRequest(QString)));
    // GROSS The newNodeRequest signal from here is monitored by mainwidget,
    // which does the instantiation, partly by calling back here,
    // and partly by calling graphWidget->addNode()
}

QGroupBox *CueLibView::createGroup(QString typeName, Node::node_t nodeType)
{
    QGroupBox *groupBox = new QGroupBox(typeName);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(2);
    layout->setContentsMargins(2,2,2,2);

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
    _signalMapper->setMapping(butt, buttonName);
    CHECKED_CONNECT(butt, SIGNAL(clicked()), _signalMapper, SLOT(map()));
}

void CueLibView::showAndRaise()
{
    show();
    raise();
}
