#include "scanpage.h"
#include "ui_scanpage.h"

ScanPage::ScanPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScanPage)
{
    ui->setupUi(this);
}

ScanPage::~ScanPage()
{
    delete ui;
}
