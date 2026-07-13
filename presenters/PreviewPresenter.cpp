#include "presenters/PreviewPresenter.h"

#include "BasicSettingPage.h"
#include "interfaces/ICameraService.h"
#include "interfaces/IStageService.h"
#include "PreviewStatusBarWidget.h"

PreviewPresenter::PreviewPresenter(BasicSettingPage *pBasicSettingPage,
    PreviewStatusBarWidget *pStatusBarWidget,
    ICameraService *pCameraService,
    IStageService *pStageService,
    QObject *parent)
    : QObject(parent)
    , m_pBasicSettingPage(pBasicSettingPage)
    , m_pStatusBarWidget(pStatusBarWidget)
    , m_pCameraService(pCameraService)
    , m_pStageService(pStageService)
{
    initConnections();
    handleCaptureSettingsChanged();
    handleStageControlChanged();
}

void PreviewPresenter::initConnections()
{
    connect(m_pBasicSettingPage, &BasicSettingPage::captureRequested,
        this, &PreviewPresenter::handleCaptureRequested);
    connect(m_pBasicSettingPage, &BasicSettingPage::recordingToggled,
        this, &PreviewPresenter::handleRecordingToggled);
    connect(m_pBasicSettingPage, &BasicSettingPage::captureSettingsChanged,
        this, &PreviewPresenter::handleCaptureSettingsChanged);
    connect(m_pBasicSettingPage, &BasicSettingPage::stageControlChanged,
        this, &PreviewPresenter::handleStageControlChanged);
    connect(m_pBasicSettingPage, &BasicSettingPage::autoStageRequested,
        this, &PreviewPresenter::handleAutoStageRequested);
}

void PreviewPresenter::handleCaptureRequested()
{
    const bool success = m_pCameraService->capture(m_pBasicSettingPage->captureSettings());
    m_pStatusBarWidget->setStatusText(success ? QStringLiteral("模拟拍照完成")
                                              : QStringLiteral("模拟拍照失败"));
}

void PreviewPresenter::handleRecordingToggled(bool checked)
{
    bool recording = false;
    if (checked)
    {
        recording = m_pCameraService->startRecord(m_pBasicSettingPage->captureSettings());
        m_pStatusBarWidget->setStatusText(recording ? QStringLiteral("模拟录像中")
                                                    : QStringLiteral("模拟录像启动失败"));
    }
    else
    {
        m_pCameraService->stopRecord();
        m_pStatusBarWidget->setStatusText(QStringLiteral("模拟录像已停止"));
    }

    m_pBasicSettingPage->setRecording(recording);
    emit recordingStateChanged(recording);
}

void PreviewPresenter::handleCaptureSettingsChanged()
{
    m_pCameraService->applyCaptureSettings(m_pBasicSettingPage->captureSettings());
    m_pBasicSettingPage->setExposureControlsEnabled(!m_pBasicSettingPage->captureSettings().autoExposure);
}

void PreviewPresenter::handleStageControlChanged()
{
    m_pStageService->moveStage(m_pBasicSettingPage->stageControlSettings());
}

void PreviewPresenter::handleAutoStageRequested()
{
    const bool success = m_pStageService->autoFocus();
    m_pStatusBarWidget->setStatusText(success ? QStringLiteral("模拟自动调焦完成")
                                              : QStringLiteral("模拟自动调焦失败"));
}
