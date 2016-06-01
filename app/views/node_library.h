// Copyright (c) 2014-2016 Alex Seiden
//
#ifndef NODE_LIBRARY_H
#define NODE_LIBRARY_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>

#include "nodes/Node.h"

// The "Cue Library" view widget.
// Displays all cues & nodes that can be instantiated.

class NodeLibrary : public QWidget
{
    Q_OBJECT
public:
    NodeLibrary(QWidget *parent = 0);

public slots:
    void        showAndRaise();

signals:
    void        newNodeRequest(QString name);

private:
    void        createUi();

    QGroupBox*  createGroup(QString typeName, Node::node_t nodeType);
    void        addButtonToUi(QLayout *layout, QString buttonName);

    QList<QPushButton *>    m_buttons;
    QSignalMapper*          m_signalMapper;
};

#endif // NODE_LIBRARY_H
