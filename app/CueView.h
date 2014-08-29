#ifndef CUEVIEW_H
#define CUEVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QPushButton>
#include <QHBoxLayout>
#include "Cue.h"
#include "CueBox.h"
#include "mainwidget.h"
#include "Whip.h"
#include "Param.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
QT_END_NAMESPACE

// Free function proto
void setButtonColor(QToolButton *colorButton, const QColor &col);


class ParamView : public QWidget
{
    Q_OBJECT
public:
    ParamView(QWidget *parent, QString name, ParamBase *param);

public slots:
    void setValue(double val);
    void setValue(int val);
    void setValue(Lightcolor val);

    void setProvider(std::function<void(float&)> closure);
//    void setProvider(std::function<void(int&)> closure);
//    void setProvider(std::function<void(Lightcolor&)> closure);

    void launchColorDialog();

private:
    QString         _name;
    ParamBase       *_param;

    Whip            *_whipButton;
    QLabel          *_label;
    QWidget         *_genericEditorWidget;
    QHBoxLayout     *_layout;       // XXX might not need to save this.
};


class CueBoxView : public QWidget
{
    Q_OBJECT
public:
    explicit CueBoxView(CueBox *cue, QWidget *parent = 0);

private:
    Cue       *_cue;  // XXX should inherit from CueView
    ParamView *_alphaParamVu;
    ParamView *_xParamVu;
    ParamView *_yParamVu;
    ParamView *_colorParamVu;
};

#endif // CUEVIEW_H
