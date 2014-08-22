#include <QtWidgets>
#include "CueView.h"

CueView::CueView(QWidget *parent) :
    QWidget(parent)
{

    this->setWindowFlags(Qt::Tool);

    alphaLabel = new QLabel(tr("alpha"));
    xLabel = new QLabel(tr("x"));
    yLabel = new QLabel(tr("y"));

    xSpinBox = new QSpinBox;
    xSpinBox->setRange(1, 18);
    xSpinBox->setSingleStep(1);

    ySpinBox = new QSpinBox;
    ySpinBox->setRange(1, 10);
    ySpinBox->setSingleStep(1);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(alphaLabel, 0, 0);
    gridLayout->addWidget(xLabel, 1, 0);
    gridLayout->addWidget(yLabel, 2, 0);
    gridLayout->addWidget(xSpinBox, 1, 1);
    gridLayout->addWidget(ySpinBox, 2, 1);
    this->setLayout(gridLayout);
}
