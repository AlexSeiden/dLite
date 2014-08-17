#include "CueSheet.h"
#include "ui_CueSheet.h"

CueSheet::CueSheet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CueSheet)
{
    ui->setupUi(this);
}

CueSheet::~CueSheet()
{
    delete ui;
}
