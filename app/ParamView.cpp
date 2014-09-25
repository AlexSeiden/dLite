#include "ParamView.h"
#include "GuiSettings.h"
#include <QtWidgets>
#include "utils.h"
#include <QLineEdit>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QAbstractSpinBox>

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
        float val = 0.0;
        floatParam->getValue(val);
#if 1
        QDoubleSpinBox *editorWidget = new QDoubleSpinBox;
        if (param->_useminmax) {    // TODO finish this
            editorWidget->setRange(param->_minVal, param->_maxVal);
            editorWidget->setSingleStep(param->_stepVal);
        }
        editorWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);

        editorWidget->setValue(val);
        CHECKED_CONNECT(editorWidget, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
#else
        QLineEdit *editorWidget = new QLineEdit;
        QDoubleValidator *validator = new QDoubleValidator(editorWidget);
        editorWidget->setValidator(validator);
        editorWidget->insert(QString::number(val));
        CHECKED_CONNECT(editorWidget, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
#endif
        _genericEditorWidget = editorWidget;
        _genericEditorWidget->setFixedSize(60,22); // XXX is this causing the zoom to break? NOPE
    }
    else if (paramType == paramTypeInt){
        Param<int> * intParam = dynamic_cast<Param<int> *>(_param);
        int val = 0;
        intParam->getValue(val);
#if 1
        QSpinBox *editorWidget = new QSpinBox;
        editorWidget->setRange(0, 10000);   // XXX need some way to set ranges etc.
        editorWidget->setSingleStep(1);
        editorWidget->setValue(val);
        CHECKED_CONNECT(editorWidget, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
#else
        QLineEdit *editorWidget = new QLineEdit;
        QIntValidator *validator = new QIntValidator(editorWidget);
        editorWidget->setValidator(validator);
        editorWidget->insert(QString::number(val));
        CHECKED_CONNECT(editorWidget, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
#endif
        _genericEditorWidget = editorWidget;
        _genericEditorWidget->setFixedSize(60,22);
    }
    else if (paramType == paramTypeLightcolor) {
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
        CHECKED_CONNECT(editorWidget, SIGNAL(stateChanged(int)), this, SLOT(setBoolValue(int)));
    }
    else if (paramType == paramTypeRegion) {
        // There's no editor widget for regions--we use the dance floor window.
        _genericEditorWidget = nullptr;
    }
    else {
        qDebug() << "ERROR: could not match typeid(param)";
        Q_ASSERT(false);
        // ErrorHandling
    }
    if (_genericEditorWidget) {
        if (param->isOutput())
            _genericEditorWidget->setEnabled(false);

        _genericEditorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        layout->addWidget(_genericEditorWidget);
    }
    this->setLayout(layout);
}


// launchColorDialog()
//      Launches a color-picker window when the color chip is clicked.
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

// ------------------------------------------------------------------------------
// setValue callbacks
//      It would be nice to template these, but that won't work with QObject
//      derived classes and the Qt moc.
//      Could use function templates?  or qvariants?
void ParamView::setValue(double val) {
    Param<float> *p = dynamic_cast<Param<float> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParent()->paramHasBeenEdited();
}

void ParamView::setValue(int val) {
    Param<int> *p = dynamic_cast<Param<int> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParent()->paramHasBeenEdited();
}

void ParamView::setValue(Lightcolor val) {
    Param<Lightcolor> *p = dynamic_cast<Param<Lightcolor> *>(this->_param);
    Q_ASSERT(p);
    p->setValue(val);
    p->getParent()->paramHasBeenEdited();
}

void ParamView::textChanged(QString text)
{
    Param<int> *pi = dynamic_cast<Param<int> *>(this->_param);
    if (pi) {
        int val = text.toInt();
        pi->setValue(val);
        pi->getParent()->paramHasBeenEdited();
        return;
    }

    Param<float> *pf = dynamic_cast<Param<float> *>(this->_param);
    if (pf) {
        float val = text.toFloat();
        pf->setValue(val);
        pf->getParent()->paramHasBeenEdited();
        return;
    }

    Q_ASSERT(false);
    // ErrorHandling
}

// Need to have this be a separate "setBoolFunction" with an int argument,
// rather than simply "setValue(bool val)", because Qt checkboxes return
// ints--Checked, unchecked, and "Partially Checked" when in a hierarchy.
void ParamView::setBoolValue(int val) {
    Param<bool> *p = dynamic_cast<Param<bool> *>(this->_param);
    Q_ASSERT(p);
    if (val == 0)
        p->setValue(false);
    else
        p->setValue(true);

    p->getParent()->paramHasBeenEdited();
}
