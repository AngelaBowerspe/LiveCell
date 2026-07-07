#include "ZStackSettingPage.h"

#include "ui_ZStackSettingPage.h"

ZStackSettingPage::ZStackSettingPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::ZStackSettingPage)
{
    m_pUi->setupUi(this);
}

ZStackSettingPage::~ZStackSettingPage()
{
    delete m_pUi;
}
