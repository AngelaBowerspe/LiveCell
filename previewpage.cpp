#include "BasicSettingPage.h"
#include "ImageCorrectionPage.h"
#include "PreviewStatusBarWidget.h"
#include "previewpage.h"
#include "SampleStageWidget.h"
#include "ui_previewpage.h"
#include "ZStackSettingPage.h"

#include <QButtonGroup>
#include <QStackedWidget>

PreviewPage::PreviewPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::PreviewPage)
    , m_pLeftNavigationGroup(new QButtonGroup(this))
    , m_pBasicSettingPage(nullptr)
    , m_pImageCorrectionPage(nullptr)
    , m_pZStackSettingPage(nullptr)
    , m_pSampleStageWidget(nullptr)
    , m_pStatusBarWidget(nullptr)
{
    m_pUi->setupUi(this);

    initPanels();
    initLeftPages();
    initNavigation();
}

PreviewPage::~PreviewPage()
{
    delete m_pUi;
}

void PreviewPage::initPanels()
{
    m_pSampleStageWidget = new SampleStageWidget(this);
    m_pStatusBarWidget = new PreviewStatusBarWidget(this);

    m_pUi->rightPanelLayout->addWidget(m_pSampleStageWidget);
    m_pUi->bottomPanelLayout->addWidget(m_pStatusBarWidget);
    m_pUi->labelRecordTime->setVisible(false);
}

void PreviewPage::initLeftPages()
{
    m_pBasicSettingPage = new BasicSettingPage(this);
    m_pImageCorrectionPage = new ImageCorrectionPage(this);
    m_pZStackSettingPage = new ZStackSettingPage(this);

    m_pUi->leftPageStackedWidget->addWidget(m_pBasicSettingPage);
    m_pUi->leftPageStackedWidget->addWidget(m_pImageCorrectionPage);
    m_pUi->leftPageStackedWidget->addWidget(m_pZStackSettingPage);
    m_pUi->leftPageStackedWidget->setCurrentWidget(m_pBasicSettingPage);
}

void PreviewPage::initNavigation()
{
    m_pLeftNavigationGroup->setExclusive(true);
    m_pLeftNavigationGroup->addButton(m_pUi->buttonBasicSettingPage, 0);
    m_pLeftNavigationGroup->addButton(m_pUi->buttonImageCorrectionPage, 1);
    m_pLeftNavigationGroup->addButton(m_pUi->buttonZStackPage, 2);

    connect(m_pLeftNavigationGroup, &QButtonGroup::idClicked,
        m_pUi->leftPageStackedWidget, &QStackedWidget::setCurrentIndex);

    m_pUi->buttonBasicSettingPage->setChecked(true);
}
