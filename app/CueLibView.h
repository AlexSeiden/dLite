#ifndef CUELIBVIEW_H
#define CUELIBVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>

#include "Node.h"

// The "Cue Library" view widget.
// Displays all cues & nodes that can be instantiated.

class CueLibView : public QWidget
{
    Q_OBJECT
public:
    CueLibView(QWidget *parent = 0);

public slots:
    void    showAndRaise();

signals:
    void    newNodeRequest(QString name);

private:
    void    createUi();

    QGroupBox*  createGroup(QString typeName, Node::node_t nodeType);
    void        addButtonToUi(QLayout *layout, QString buttonName);

    QList<QPushButton *>    m_buttons;
    QSignalMapper*          m_signalMapper;
};

#endif // CUELIBVIEW_H
