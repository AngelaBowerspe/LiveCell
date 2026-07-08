#ifndef PREVIEWPRESENTER_H
#define PREVIEWPRESENTER_H

#include <QObject>

class BasicSettingPage;
class ICameraService;
class IStageService;
class PreviewStatusBarWidget;

class PreviewPresenter : public QObject
{
    Q_OBJECT

public:
    explicit PreviewPresenter(BasicSettingPage *pBasicSettingPage,
        PreviewStatusBarWidget *pStatusBarWidget,
        ICameraService *pCameraService,
        IStageService *pStageService,
        QObject *parent = nullptr);

signals:
    void recordingStateChanged(bool recording);

private:
    void initConnections();
    void handleCaptureRequested();
    void handleRecordingToggled(bool checked);
    void handleCaptureSettingsChanged();
    void handleStageControlChanged();
    void handleAutoStageRequested();

private:
    BasicSettingPage *m_pBasicSettingPage;
    PreviewStatusBarWidget *m_pStatusBarWidget;
    ICameraService *m_pCameraService;
    IStageService *m_pStageService;
};

#endif // PREVIEWPRESENTER_H
