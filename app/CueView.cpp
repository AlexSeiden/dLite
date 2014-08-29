#include <QtWidgets>
#include "CueView.h"
#include "utils.h"

CueView::CueView(Cue *cue, QWidget *parent) :
    QWidget(parent),
    _cue(cue)
{
    setWindowFlags(Qt::Tool);
    setWindowTitle(_cue->getName());
}

CueBoxView::CueBoxView(CueBox *cue, QWidget *parent) :
    QWidget(parent),
    _cue(cue)
{
    setWindowFlags(Qt::Tool);
    setWindowTitle(_cue->getName());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(8);
    _alphaParamVu = new ParamView(this, tr("alpha"), &cue->_alpha);
    _xParamVu = new ParamView(this, tr("x"), &cue->_xoffset);
    _yParamVu = new ParamView(this, tr("y"), &cue->_yoffset);
    _colorParamVu = new ParamView(this, tr("color"), &cue->_color);
    layout->addWidget(_alphaParamVu);
    layout->addWidget(_xParamVu);
    layout->addWidget(_yParamVu);
    layout->addWidget(_colorParamVu);
    layout->addStretch();

    this->setLayout(layout);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->updateGeometry();

}



void setButtonColor(QToolButton *colorButton, const QColor &col){
    if(col.isValid()) {
        // I grabbed this bit of black magic code from StackOverflow...
        // seems like the only way to set the color of the button.
        QString qss = QString("background-color: %1").arg(col.name());
        colorButton->setStyleSheet(qss);
#if 0
        if (_cue)       // XXX should assert(_cue)
            _cue->_color = Lightcolor(col);
#endif
    }
}

ParamView::ParamView(QWidget *parent, QString name, ParamBase *param) :
    QWidget(parent),
    _name(name),
    _param(param)
{
    _layout = new QHBoxLayout;
    _whipButton = new Whip(_param, this);
    _label = new QLabel(_name);
    _label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _label->setMinimumSize(50,20);
    _label->setContentsMargins(5,0,5,0);
    const std::type_info & paramType = typeid(*param);
    if (paramType == paramTypeFloat) {
        Param<float> * floatParam = dynamic_cast<Param<float> *>(_param);
        QDoubleSpinBox *editorWidget = new QDoubleSpinBox;
        editorWidget->setRange(0, 1.0);
        editorWidget->setSingleStep(.01);
        _genericEditorWidget = editorWidget;
        float val = 0.0;
        floatParam->getValue(val);
        editorWidget->setValue(val);
        _genericEditorWidget->setFixedSize(60,22);

        CHECKED_CONNECT(editorWidget, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
    }
    else if (paramType == paramTypeInt){
        Param<int> * intParam = dynamic_cast<Param<int> *>(_param);
        QSpinBox *editorWidget = new QSpinBox;
        editorWidget->setRange(1, 10);
        editorWidget->setSingleStep(1);
        _genericEditorWidget = editorWidget;
        _genericEditorWidget->setFixedSize(60,22);
        int val = 0;
        intParam->getValue(val);
        editorWidget->setValue(val);
        CHECKED_CONNECT(editorWidget, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    }
    else if (paramType == paramTypeLcolor) {
        Param<Lightcolor> * colorParam = dynamic_cast<Param<Lightcolor> *>(_param);
        QToolButton *editorWidget =  new QToolButton(this);
        _genericEditorWidget = editorWidget;
        Lightcolor val;
        colorParam->getValue(val);
        QColor qc = val.toQColor();
        setButtonColor(editorWidget, qc);

        CHECKED_CONNECT(editorWidget, SIGNAL(clicked()), this, SLOT(launchColorDialog()));
    }
    else
        qDebug() << "ERROR: could not match typeid(param)";
        // TODO handle error better

    _genericEditorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _layout->setSpacing(0);
    _layout->setContentsMargins(8,0,8,0);
    _layout->addWidget(_whipButton);
    _layout->addWidget(_label);
    _layout->addWidget(_genericEditorWidget);
    _layout->addStretch();
    this->setLayout(_layout);
}

void ParamView::launchColorDialog() {
    QToolButton *colorButton = qobject_cast<QToolButton *>(this->_genericEditorWidget);
    Param<Lightcolor> * colorParam = dynamic_cast<Param<Lightcolor> *>(_param);
    Lightcolor val;
    colorParam->getValue(val);
    QColor qc = val.toQColor();
    QColor outcol = QColorDialog::getColor(qc, NULL);
    if (! outcol.isValid())
        return;

    val = Lightcolor(outcol);
    emit setValue(val);
    setButtonColor(colorButton, outcol);
}

void ParamView::setValue(double val){
   Param<float> *p = dynamic_cast<Param<float> *>(this->_param);
   Q_ASSERT(p);
   p->setValue(val);
}

void ParamView::setValue(int val){
   Param<int> *p = dynamic_cast<Param<int> *>(this->_param);
   Q_ASSERT(p);
   p->setValue(val);
}

void ParamView::setValue(Lightcolor val){
   Param<Lightcolor> *p = dynamic_cast<Param<Lightcolor> *>(this->_param);
   Q_ASSERT(p);
   p->setValue(val);
}
