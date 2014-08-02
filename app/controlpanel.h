#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include "sublevel.h"
#include "engine.h"

class Controlpanel : public QWidget
{
    Q_OBJECT
public:
    explicit Controlpanel(QWidget *parent = 0, Engine *engine = NULL);

    // QWidget
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void submeterHasBeenSelected(SublevelMeter *chosen);

signals:
    void submeterSelectionChanged(SublevelMeter *chosen);


private:
    void createUi();
    void connectUi();
    void addMeter();
    SublevelMeter *submeterSelected();


    Engine  *m_engine;
    QHBoxLayout *hLayout;
    int numMeters;
    QList<SublevelMeter *>  meters;
};

#endif // CONTROLPANEL_H
