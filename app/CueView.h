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

// Free function proto
void setButtonColor(QToolButton *colorButton, const QColor &col);

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

class RandomNodeView : public QWidget
{
    Q_OBJECT
public:
    explicit RandomNodeView(RandomNode *randomNode, QWidget *parent = 0);

private:
    RandomNode *_node;  // ??? should inherit from NodeView base class?
    ParamView *_outParamVu;
    ParamView *_minParamVu;
    ParamView *_maxParamVu;
};

#endif // CUEVIEW_H
