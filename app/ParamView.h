#ifndef PARAMVIEW_H
#define PARAMVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include "Cue.h"
#include "Node.h"
#include "CueBox.h"
#include "Whip.h"
#include "Param.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ParamView : public QWidget
{
    Q_OBJECT
public:
    ParamView(QWidget *parent, QString name, ParamBase *param, bool inputConnection=true);

public slots:
    // It would be nice to template these, but that won't work with QObject
    // derived classes and the Qt moc.
    void setValue(double val);
    void setValue(int val);
    void setValue(Lightcolor val);

    void setProvider(std::function<void(float&)> closure);
    void setProvider(std::function<void(int&)> closure);
    void setProvider(std::function<void(Lightcolor&)> closure);

    void launchColorDialog();

private:
    QString         _name;
    ParamBase       *_param;

    Whip            *_whipButton;
    QLabel          *_label;
    QWidget         *_genericEditorWidget;
    QHBoxLayout     *_layout;       // XXX might not need to save this.
    bool            _inputConnection; // Stores whether this is an input or output connection
};

#endif // PARAMVIEW_H
