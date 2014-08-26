#include <QtWidgets>
#include "CueView.h"
#include "utils.h"

CueView::CueView(Cue *cue, QWidget *parent) :
    QWidget(parent),
    _cue(cue)
{
    //this->setWindowFlags(Qt::Tool);

    alphaLabel = new QLabel(tr("alpha"));
    xLabel = new QLabel(tr("x"));
    yLabel = new QLabel(tr("y"));
    colorLabel = new QLabel(tr("color"));
    colorButton = new QToolButton(this);
    setButtonColor(_cue->_color.toQColor());

    alphaSpinBox = new QDoubleSpinBox;
    alphaSpinBox->setRange(0, 1.0);
    alphaSpinBox->setSingleStep(.01);

    xSpinBox = new QSpinBox;
    xSpinBox->setRange(1, 18);
    xSpinBox->setSingleStep(1);

    ySpinBox = new QSpinBox;
    ySpinBox->setRange(1, 10);
    ySpinBox->setSingleStep(1);

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

    CHECKED_CONNECT(colorButton, SIGNAL(clicked()), this, SLOT(setColorDialog()));

    setWindowTitle(_cue->getName());
}

void CueView::setColorDialog(){
    QColor col = QColorDialog::getColor(_cue->_color.toQColor(), this);
    setButtonColor(col);
}

void CueView::setButtonColor(const QColor &col){
    if(col.isValid()) {
        QString qss = QString("background-color: %1").arg(col.name());
        colorButton->setStyleSheet(qss);
        if (_cue)       // XXX should assert(_cue)
            _cue->_color = Lightcolor(col);
    }
}
