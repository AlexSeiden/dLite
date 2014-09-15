#include <QtWidgets>
#include "CueView.h"
#include "GuiColors.h"
#include "utils.h"

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
