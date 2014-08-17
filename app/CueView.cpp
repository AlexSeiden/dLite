#include "CueView.h"
#include "ui_CueView.h"

CueView::CueView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CueView)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool);
}

CueView::~CueView()
{
    delete ui;
}
