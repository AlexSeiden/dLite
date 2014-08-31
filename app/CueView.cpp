#include <QtWidgets>
#include "CueView.h"
#include "utils.h"


// Utility function
void setButtonColor(QToolButton *colorButton, const QColor &col){
    if(col.isValid()) {
        // I grabbed this bit of black magic code from StackOverflow...
        // seems like the only way to set the color of the button.
        QString qss = QString("background-color: %1").arg(col.name());
        colorButton->setStyleSheet(qss);
    }
}


CueBoxView::CueBoxView(CueBox *cue, QWidget *parent) :
    QWidget(parent),
    _cue(cue)
{
    setWindowFlags(Qt::Tool | Qt::WindowTitleHint  |
                   Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setWindowTitle(_cue->getName());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(8);
    _alphaParamVu = new ParamView(this, tr("alpha"), &_cue->_alpha);
    _xParamVu = new ParamView(this, tr("x"), &_cue->_x);
    _yParamVu = new ParamView(this, tr("y"), &_cue->_y);
    _colorParamVu = new ParamView(this, tr("color"), &_cue->_color);
    layout->addWidget(_alphaParamVu);
    layout->addWidget(_xParamVu);
    layout->addWidget(_yParamVu);
    layout->addWidget(_colorParamVu);
    layout->addStretch();

    this->setLayout(layout);
    // XXX: for some reason, the widgets are still resizable.
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

// ------------------------------------------------------------------------------

RandomNodeView::RandomNodeView(RandomNode *randomNode, QWidget *parent) :
    QWidget(parent),
    _node(randomNode)
{
    setWindowFlags(Qt::Tool | Qt::WindowTitleHint  |
                   Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setWindowTitle(_node->getName());
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(8);

    _outParamVu = new ParamView(this, tr("Output"), &_node->_min, false);
    _minParamVu = new ParamView(this, tr("min"), &_node->_min);
    _maxParamVu = new ParamView(this, tr("max"), &_node->_max);
    layout->addWidget(_outParamVu);
    layout->addWidget(_minParamVu);
    layout->addWidget(_maxParamVu);
    layout->addStretch();
    this->setLayout(layout);
}
