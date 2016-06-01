// Copyright (c) 2014-2016 Alex Seiden
//
// ------------------------------------------------------------------------------
// node_library.cpp
// Widget that allows the user to create new nodes.
//
// TODO turn this into a toolbar that's much more space efficient.
// TODO be able to add group nodes etc. to this.

#include <QtWidgets>

#include "views/node_library.h"
#include "engine/nodefactory.h"
#include "engine/utils.h"

NodeLibrary::NodeLibrary(QWidget *parent) :
    QWidget(parent)
{
    createUi();
    move(610, 10);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}


void NodeLibrary::createUi()
{
    m_signalMapper = new QSignalMapper(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);

    mainLayout->addWidget(createGroup("Shapes", Node::CUE));
    mainLayout->addWidget(createGroup("Ints", Node::INT));
    mainLayout->addWidget(createGroup("Floats", Node::FLOAT));
    mainLayout->addWidget(createGroup("Colors", Node::COLOR));
    mainLayout->addWidget(createGroup("Beats", Node::BEAT));
    mainLayout->addWidget(createGroup("Regions", Node::REGION));
    setLayout(mainLayout);

    setObjectName(tr("NodeLib"));
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint);
    CHECKED_CONNECT(m_signalMapper, SIGNAL(mapped(QString)),
                    this, SIGNAL(newNodeRequest(QString)));
    // GROSS The newNodeRequest signal from here is monitored by mainwidget,
    // which does the instantiation, partly by calling back here,
    // and partly by calling graphWidget->addNode()
}

QGroupBox *NodeLibrary::createGroup(QString typeName, Node::node_t nodeType)
{
    QGroupBox *groupBox = new QGroupBox(typeName);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->setMargin(0);
    layout->setAlignment(Qt::AlignLeft);

    QStringList allNodenames = NodeFactory::Singleton()->getNodesOfType(nodeType);

    foreach (QString nodename, allNodenames)
        addButtonToUi(layout, nodename);

    groupBox->setLayout(layout);
    return groupBox;
}

// Convienence function
void NodeLibrary::addButtonToUi(QLayout *layout, QString buttonName) {
    QPushButton *butt = new QPushButton(this);
    butt->setObjectName("CueButt");
    butt->setText(buttonName);
    m_buttons.push_back(butt);
    layout->addWidget(butt);
    m_signalMapper->setMapping(butt, buttonName);
    CHECKED_CONNECT(butt, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
}

void NodeLibrary::showAndRaise()
{
    show();
    raise();
}

// ------------------------------------------------------------------------------
