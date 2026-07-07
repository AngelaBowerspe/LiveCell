#include "BasicSettingPage.h"

#include "ui_BasicSettingPage.h"

#include <QPushButton>

BasicSettingPage::BasicSettingPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::BasicSettingPage)
{
    m_pUi->setupUi(this);

    connect(m_pUi->buttonRecordVideo, &QPushButton::toggled, this, [this](bool checked) {
        m_pUi->buttonRecordVideo->setText(checked ? QStringLiteral("停止录像") : QStringLiteral("录像"));
    });
    connect(m_pUi->buttonRecordVideo, &QPushButton::toggled,
        this, &BasicSettingPage::recordVideoToggled);
}

BasicSettingPage::~BasicSettingPage()
{
    delete m_pUi;
}
