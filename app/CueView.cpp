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

#if 0
CueBoxView::CueBoxView(Cue *cue, QWidget *parent)
{
    alphaLabel = new QLabel(tr("alpha"));
    xLabel = new QLabel(tr("x"));
    yLabel = new QLabel(tr("y"));
    colorLabel = new QLabel(tr("color"));
    colorButton = new QToolButton(this);
    setButtonColor(_cue->_color.toQColor());

    whipButton = new Whip(&_cue->_alpha, this);  // TODO generalize

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(whipButton,   0, 0);
    gridLayout->addWidget(alphaLabel,   0, 1);
    gridLayout->addWidget(xLabel,       1, 1);
    gridLayout->addWidget(yLabel,       2, 1);
    gridLayout->addWidget(colorLabel,   3, 1);
    gridLayout->addWidget(alphaSpinBox, 0, 2);
    gridLayout->addWidget(xSpinBox,     1, 2);
    gridLayout->addWidget(ySpinBox,     2, 2);
    gridLayout->addWidget(colorButton,  3, 2);
    this->setLayout(gridLayout);

    CHECKED_CONNECT(colorButton, SIGNAL(clicked()), this, SLOT(launchColorDialog()));
}
#else
CueBoxView::CueBoxView(CueBox *cue, QWidget *parent) :
    QWidget(parent),
    _cue(cue)
{
    QVBoxLayout *layout = new QVBoxLayout;
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

}
#endif


#if 0
void CueView::launchColorDialog(){
    QColor col = QColorDialog::getColor(_cue->_color.toQColor(), this);
    setButtonColor(col);
}

void CueView::setButtonColor(QToolButton &colorButton, const QColor &col){
    if(col.isValid()) {
        // I grabbed this bit of black magic code from StackOverflow...
        // seems like the only way to set the color of the button.
        QString qss = QString("background-color: %1").arg(col.name());
        colorButton->setStyleSheet(qss);
        if (_cue)       // XXX should assert(_cue)
            _cue->_color = Lightcolor(col);
    }
}
#endif

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
    const std::type_info & paramType = typeid(*param);
    const std::type_info & paramFloat = typeid(Param<float>);
    const std::type_info & paramInt = typeid(Param<int>);
    const std::type_info & paramLcolor = typeid(Param<Lightcolor>);
    if (paramType == paramFloat) {
        Param<float> * floatParam = dynamic_cast<Param<float> *>(_param);
        QDoubleSpinBox *editorWidget = new QDoubleSpinBox;
        editorWidget->setRange(0, 1.0);
        editorWidget->setSingleStep(.01);
        _genericEditorWidget = editorWidget;
        float val = 0.0;
        floatParam->getValue(val);
        editorWidget->setValue(val);

        //CHECKED_CONNECT(this, SIGNAL(valueChanged(double)), _param, SLOT(setValue(double)));
    }
    else if (paramType == paramInt){
        Param<int> * intParam = dynamic_cast<Param<int> *>(_param);
        QSpinBox *editorWidget = new QSpinBox;
        editorWidget->setRange(1, 10);
        editorWidget->setSingleStep(1);
        _genericEditorWidget = editorWidget;
        int val = 0;
        intParam->getValue(val);
        editorWidget->setValue(val);
        //CHECKED_CONNECT(this, SIGNAL(valueChanged(int)), _param, SLOT(setValue(int)));
    }
    else if (paramType == paramLcolor) {
        Param<Lightcolor> * colorParam = dynamic_cast<Param<Lightcolor> *>(_param);
        QToolButton *editorWidget =  new QToolButton(this);
        _genericEditorWidget = editorWidget;
        Lightcolor val;
        colorParam->getValue(val);
        //setButtonColor(editorWidget, val.toQColor());

        //CHECKED_CONNECT(editorWidget, SIGNAL(clicked()), this, SLOT(launchColorDialog()));
    }
    else
        qDebug() << "ERROR: could not match typeid(param)";

    _genericEditorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _genericEditorWidget->setMinimumSize(50,20);

    _layout->setSpacing(0);
    _layout->setContentsMargins(0,0,0,0);
    _layout->addWidget(_whipButton);
    _layout->addWidget(_label);
    _layout->addWidget(_genericEditorWidget);
    _layout->addStretch();
    this->setLayout(_layout);
}
