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

signals:
    void    newNodeRequest(QString name);

private:
    void    createUi();
#if 0
    void    createCuesGroup();
    void    createFloatsGroup();
    void    createColorsGroup();
    void    createBeatsGroup();
#endif

    QGroupBox   *createGroup(QString typeName, Node::node_t nodeType);
    void    addButtonToUi(QLayout *layout, QString buttonName);


#if 0
    QGroupBox *cuesGroupBox;
    QGroupBox *floatsGroupBox;
    QGroupBox *colorsGroupBox;
    QGroupBox *beatsGroupBox;
#endif

    QList<QPushButton *> buttons;

    QHBoxLayout *mainLayout;
    QSignalMapper *signalMapper;

};

#endif // CUELIBVIEW_H
