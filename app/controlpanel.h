#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include "sublevel.h"
#include "engine.h"
#include "Cue.h"
#include "CueView.h"

class Controlpanel : public QWidget
{
    Q_OBJECT
public:
    explicit Controlpanel(QWidget *parent,
                          Engine *engine,
                          Dancefloormodel *dfmodel);

    // QWidget
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void submeterHasBeenSelected(SublevelMeter *chosen);
    void addSensor();
    void addCue();

signals:
    void submeterSelectionChanged(SublevelMeter *chosen);

private:
    void createUi();
    SublevelMeter *submeterSelected();

    QPushButton*            m_addSensorButton;
    QIcon                   m_addSensorIcon;
    QPushButton*            m_addCueButton;

    QVBoxLayout               *windowLayout;
    QHBoxLayout               *hLayout;
    QWidget                   *controlsPanel;

    int                       numMeters;
    QList< SublevelMeter * >  meters;

    Engine *                   _engine;
    Dancefloormodel *          _dfModel;
};

#endif // CONTROLPANEL_H
