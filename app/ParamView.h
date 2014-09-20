#ifndef PARAMVIEW_H
#define PARAMVIEW_H

// ParamView.h
//  A widget that displays a single param.

#include <QWidget>
#include <QHBoxLayout>
#include "Cue.h"
#include "Node.h"
#include "CueBox.h"
#include "Param.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ParamView : public QWidget
{
    Q_OBJECT
public:
    ParamView(QWidget *parent, QString name, ParamBase *param);
    ParamView(QWidget *parent, ParamBase *param);

public slots:
    // It would be nice to template these, but that won't work with QObject
    // derived classes and the Qt moc.
    void setValue(double val);
    void setBoolValue(int val);
    void setValue(int val);
    void setValue(Lightcolor val);
    void textChanged(QString val);

    void launchColorDialog();

private:
    QString         _name;
    ParamBase       *_param;

    QLabel          *_label;
    QWidget         *_genericEditorWidget;
};

#endif // PARAMVIEW_H
