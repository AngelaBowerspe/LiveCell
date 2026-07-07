#include "BasicSettingPage.h"

#include "ui_BasicSettingPage.h"

BasicSettingPage::BasicSettingPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::BasicSettingPage)
{
    m_pUi->setupUi(this);
}

BasicSettingPage::~BasicSettingPage()
{
    delete m_pUi;
}
