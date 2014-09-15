#ifndef CUEVIEW_H
#define CUEVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QPushButton>
#include <QHBoxLayout>
#include "Cue.h"
#include "Node.h"
#include "CueBox.h"
#include "mainwidget.h"
#include "ParamView.h"

class CueBoxView : public QWidget
{
    Q_OBJECT
public:
    explicit CueBoxView(CueBox *cue, QWidget *parent = 0);

private:
    CueBox    *_cue;  // ??? should inherit from CueView base class?
    ParamView *_alphaParamVu;
    ParamView *_xParamVu;
    ParamView *_yParamVu;
    ParamView *_colorParamVu;
};

#endif // CUEVIEW_H
