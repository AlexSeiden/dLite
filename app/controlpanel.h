#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include "sublevel.h"

class Controlpanel : public QWidget
{
    Q_OBJECT
public:
    explicit Controlpanel(QWidget *parent = 0);

    // QWidget
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void submeterHasBeenSelected(SublevelMeter *chosen);

private:
    void createUi();
    void connectUi();
    void addMeter();
    SublevelMeter *submeterSelected();


    QHBoxLayout *hLayout;
    int numMeters;
    QList<SublevelMeter *>  meters;
};

#endif // CONTROLPANEL_H
