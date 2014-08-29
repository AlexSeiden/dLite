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

QT_BEGIN_NAMESPACE
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
QT_END_NAMESPACE

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

    void launchColorDialog();

private:
    QString         _name;
    ParamBase       *_param;

    Whip            *_whipButton;
    QLabel          *_label;
    QWidget         *_genericEditorWidget;
    QHBoxLayout     *_layout;       // XXX might not need to save this.
};

class CueView : public QWidget
{
    Q_OBJECT
public:
    explicit CueView(Cue *cue, QWidget *parent = 0);

signals:

public slots:

protected:
    Cue     *_cue;

private:
    void    addFloatParam(QString name);

    void setButtonColor(QToolButton &colorButton, const QColor &col);
};


class CueBoxView : public QWidget
{
    Q_OBJECT
public:
    explicit CueBoxView(CueBox *cue, QWidget *parent = 0);

signals:

public slots:
#if 0
    void launchColorDialog();
#endif

private:
    Cue       *_cue;  // XXX should inherit from CueView
    ParamView *_alphaParamVu;
    ParamView *_xParamVu;
    ParamView *_yParamVu;
    ParamView *_colorParamVu;

    //void setButtonColor(QToolButton &colorButton, const QColor &col);
};

#endif // CUEVIEW_H
