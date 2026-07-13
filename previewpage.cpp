#include "BasicSettingPage.h"
#include "ImageCorrectionPage.h"
#include "mocks/MockCameraService.h"
#include "mocks/MockStageService.h"
#include "PreviewStatusBarWidget.h"
#include "presenters/PreviewPresenter.h"
#include "previewpage.h"
#include "SampleStageWidget.h"
#include "ui_previewpage.h"
#include "ZStackSettingPage.h"

#include <QButtonGroup>
#include <QElapsedTimer>
#include <QStackedWidget>
#include <QTimer>

PreviewPage::PreviewPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::PreviewPage)
    , m_pLeftNavigationGroup(new QButtonGroup(this))
    , m_pRecordTimer(new QTimer(this))
    , m_pRecordElapsedTimer(new QElapsedTimer)
    , m_pBasicSettingPage(nullptr)
    , m_pImageCorrectionPage(nullptr)
    , m_pZStackSettingPage(nullptr)
    , m_pSampleStageWidget(nullptr)
    , m_pStatusBarWidget(nullptr)
    , m_pPreviewPresenter(nullptr)
{
    m_pUi->setupUi(this);

    initPanels();
    initLeftPages();
    initNavigation();
}

PreviewPage::~PreviewPage()
{
    delete m_pRecordElapsedTimer;
    delete m_pUi;
}

ObjectiveMagnification PreviewPage::objectiveMagnification() const
{
    if (m_pBasicSettingPage == nullptr)
    {
        return ObjectiveMagnification::Objective10X;
    }

    return m_pBasicSettingPage->captureSettings().objective;
}

void PreviewPage::initPanels()
{
    m_pSampleStageWidget = new SampleStageWidget(this);
    m_pStatusBarWidget = new PreviewStatusBarWidget(this);

    m_pUi->rightPanelLayout->addWidget(m_pSampleStageWidget);
    m_pUi->bottomPanelLayout->addWidget(m_pStatusBarWidget);
    m_pUi->labelRecordTime->setText(QStringLiteral("00:00:00"));
    m_pUi->labelRecordTime->setVisible(false);

    m_pRecordTimer->setInterval(1000);
    connect(m_pRecordTimer, &QTimer::timeout, this, &PreviewPage::updateRecordTime);
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

    m_pCameraService = std::make_unique<MockCameraService>();
    m_pStageService = std::make_unique<MockStageService>();
    m_pPreviewPresenter = new PreviewPresenter(m_pBasicSettingPage,
        m_pStatusBarWidget,
        m_pCameraService.get(),
        m_pStageService.get(),
        this);

    connect(m_pPreviewPresenter, &PreviewPresenter::recordingStateChanged,
        this, &PreviewPage::setRecordingEnabled);
    connect(m_pBasicSettingPage, &BasicSettingPage::captureSettingsChanged,
        this, [this]() {
            emit objectiveMagnificationChanged(objectiveMagnification());
        });
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

void PreviewPage::setRecordingEnabled(bool enabled)
{
    if (enabled)
    {
        m_pRecordElapsedTimer->restart();
        m_pUi->labelRecordTime->setText(QStringLiteral("00:00:00"));
        m_pUi->labelRecordTime->setVisible(true);
        m_pRecordTimer->start();
        return;
    }

    m_pRecordTimer->stop();
    m_pUi->labelRecordTime->setVisible(false);
    m_pUi->labelRecordTime->setText(QStringLiteral("00:00:00"));
}

void PreviewPage::updateRecordTime()
{
    const qint64 totalSeconds = m_pRecordElapsedTimer->elapsed() / 1000;
    const qint64 hours = totalSeconds / 3600;
    const qint64 minutes = (totalSeconds % 3600) / 60;
    const qint64 seconds = totalSeconds % 60;

    m_pUi->labelRecordTime->setText(QStringLiteral("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0')));
}
