#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include "sublevel.h"
#include "engine.h"
#include "cue.h"

class Controlpanel : public QWidget
{
    Q_OBJECT
public:
    explicit Controlpanel(QWidget *parent, Engine *engine, Dancefloormodel *dfmodel);

    // QWidget
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void submeterHasBeenSelected(SublevelMeter *chosen);

signals:
    void submeterSelectionChanged(SublevelMeter *chosen);

private:
    void createUi();
    void addMeter();
    SublevelMeter *submeterSelected();


    QHBoxLayout               *hLayout;

    int                       numMeters;
    QList< SublevelMeter * >  meters;

    Engine *                   _engine;
    Dancefloormodel *          _dfmodel;
    Cue *                      _cue;
};

#endif // CONTROLPANEL_H
