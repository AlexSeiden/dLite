#ifndef CUEVIEW_H
#define CUEVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QPushButton>
#include "Cue.h"
#include "mainwidget.h"
#include "Whip.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
QT_END_NAMESPACE

class CueView : public QWidget
{
    Q_OBJECT
public:
    explicit CueView(Cue *cue, QWidget *parent = 0);

signals:

public slots:
    void setColorDialog();

private:
    //MainWidget *_mw;

    QLabel *alphaLabel;
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *colorLabel;
    QDoubleSpinBox *alphaSpinBox;
    QSpinBox *xSpinBox;
    QSpinBox *ySpinBox;
    QToolButton *colorButton;
    Cue     *_cue;
    Whip *whipButton;
    QIcon whipIcon;


    void setButtonColor(const QColor &col);
};

#endif // CUEVIEW_H
