#include "ParamView.h"
#include "GuiColors.h"
#include <QtWidgets>
#include "utils.h"

// This is the param view for use in the graph view
// TODO unify these
ParamView::ParamView(QWidget *parent, ParamBase *param ) :
    QWidget(parent),
    _name(param->getName()),
    _param(param)
{
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(8,0,8,0);

    const std::type_info & paramType = typeid(*param);

    // TODO make range & steps on spinboxes settable
   if (paramType == paramTypeFloat) {
        Param<float> * floatParam = dynamic_cast<Param<float> *>(_param);
        QDoubleSpinBox *editorWidget = new QDoubleSpinBox;
        editorWidget->setRange(0, 1.0);
        editorWidget->setSingleStep(.01);
        _genericEditorWidget = editorWidget;
        float val = 0.0;
        floatParam->getValue(val);
        editorWidget->setValue(val);
        _genericEditorWidget->setFixedSize(60,22); // XXX is this causing the zoom to break? NOPE
        CHECKED_CONNECT(editorWidget, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
    }
    else if (paramType == paramTypeInt){
        Param<int> * intParam = dynamic_cast<Param<int> *>(_param);
        QSpinBox *editorWidget = new QSpinBox;
        editorWidget->setRange(0, 10000);   // XXX need some way to set ranges etc.
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
    else if (paramType == paramTypeBool) {
        Param<bool> * boolParam = dynamic_cast<Param<bool> *>(_param);
        QCheckBox *editorWidget =  new QCheckBox(this);
        _genericEditorWidget = editorWidget;
        bool val;
        boolParam->getValue(val);
        editorWidget->setChecked(val);
        // XXX below we're calling setValue(int) when we should probly call setValue(bool)
        CHECKED_CONNECT(editorWidget, SIGNAL(stateChanged(int)), this, SLOT(setValue(int)));
    }
    else {
        qDebug() << "ERROR: could not match typeid(param)";
        Q_ASSERT(false);
        // ErrorHandling
   }

    if (param->isOutput())
        _genericEditorWidget->setEnabled(false);

    _genericEditorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(_genericEditorWidget);
    this->setLayout(layout);
}


// This is the param view for use in editor widgets
ParamView::ParamView(QWidget *parent, QString name, ParamBase *param) :
    QWidget(parent),
    _name(name),
    _param(param)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(8,0,8,0);

    _whipButton = new Whip(_param, this);

    _label = new QLabel(_name);
    _label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _label->setMinimumSize(50,20);
    _label->setContentsMargins(5,0,5,0);

    const std::type_info & paramType = typeid(*param);

    // TODO make range & steps on spinboxes settable
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
        editorWidget->setRange(1, 20);
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
        // ErrorHandling

    _genericEditorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if (! param->isOutput()) {
        layout->addWidget(_whipButton);
        layout->addWidget(_label);
        layout->addWidget(_genericEditorWidget);
    }
    else {
        layout->addSpacing(_whipButton->width());
        layout->addWidget(_label);
        layout->addWidget(_whipButton);
    }
    layout->addStretch();
    this->setLayout(layout);
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

// It would be nice to template these, but that won't work with QObject
// derived classes and the Qt moc.
void ParamView::setValue(double val) {
    Param<float> *p = dynamic_cast<Param<float> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->_parentNode->paramHasBeenEdited();
}

void ParamView::setValue(int val) {
    Param<int> *p = dynamic_cast<Param<int> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->_parentNode->paramHasBeenEdited();
}

void ParamView::setValue(Lightcolor val) {
    Param<Lightcolor> *p = dynamic_cast<Param<Lightcolor> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->_parentNode->paramHasBeenEdited();
}

#ifdef NUKEME
void ParamView::setProvider(std::function<void()> closure) {
   this->setProvider(closure);
}

void ParamView::setProvider(std::function<void()> closure) {
   Param<int> *p = dynamic_cast<Param<int> *>(this->_param);
   Q_ASSERT(p);
   p->setProvider(closure);
}

void ParamView::setProvider(std::function<void(Lightcolor&)> closure) {
   Param<Lightcolor> *p = dynamic_cast<Param<Lightcolor> *>(this->_param);
   Q_ASSERT(p);
   p->setProvider(closure);
}
#endif
