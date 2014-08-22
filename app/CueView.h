#ifndef CUEVIEW_H
#define CUEVIEW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QSpinBox;
QT_END_NAMESPACE

class CueView : public QWidget
{
    Q_OBJECT
public:
    explicit CueView(QWidget *parent = 0);

signals:

public slots:

private:
    QLabel *alphaLabel;
    QLabel *xLabel;
    QLabel *yLabel;
    QSpinBox *xSpinBox;
    QSpinBox *ySpinBox;
};

#endif // CUEVIEW_H
