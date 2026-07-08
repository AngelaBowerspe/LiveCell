#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include <QButtonGroup>
#include <QElapsedTimer>
#include <QTimer>
#include <QWidget>

#include <memory>

class BasicSettingPage;
class ICameraService;
class IStageService;
class ImageCorrectionPage;
class PreviewStatusBarWidget;
class PreviewPresenter;
class SampleStageWidget;
class ZStackSettingPage;

namespace Ui {
class PreviewPage;
}

class PreviewPage : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewPage(QWidget *parent = nullptr);
    ~PreviewPage();

private:
    void initLeftPages();
    void initNavigation();
    void initPanels();
    void setRecordingEnabled(bool enabled);
    void updateRecordTime();

private:
    Ui::PreviewPage *m_pUi;
    QButtonGroup *m_pLeftNavigationGroup;
    QTimer *m_pRecordTimer;
    QElapsedTimer *m_pRecordElapsedTimer;
    BasicSettingPage *m_pBasicSettingPage;
    ImageCorrectionPage *m_pImageCorrectionPage;
    ZStackSettingPage *m_pZStackSettingPage;
    SampleStageWidget *m_pSampleStageWidget;
    PreviewStatusBarWidget *m_pStatusBarWidget;
    PreviewPresenter *m_pPreviewPresenter;
    std::unique_ptr<ICameraService> m_pCameraService;
    std::unique_ptr<IStageService> m_pStageService;
};

#endif // PREVIEWPAGE_H
