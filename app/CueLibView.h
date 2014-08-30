#ifndef CUELIBVIEW_H
#define CUELIBVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>

class CueLibView : public QWidget
{
    Q_OBJECT
public:
    CueLibView(QWidget *parent = 0);

signals:
    void    newNodeRequest(QString name);

private:
    void    createUi();
    void    createCuesGroup();
    void    createFloatsGroup();
    void    createColorsGroup();
    void    createBeatsGroup();
    void    addButtonToUi(QLayout *layout, QString buttonName);

    QGroupBox *cuesGroupBox;
    QGroupBox *floatsGroupBox;
    QGroupBox *colorsGroupBox;
    QGroupBox *beatsGroupBox;
    QList<QPushButton *> buttons;

    QHBoxLayout *mainLayout;
    QSignalMapper *signalMapper;

};

#endif // CUELIBVIEW_H
