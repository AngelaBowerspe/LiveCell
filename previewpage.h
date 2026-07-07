#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include <QWidget>

class BasicSettingPage;
class ImageCorrectionPage;
class PreviewStatusBarWidget;
class QButtonGroup;
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

private:
    Ui::PreviewPage *m_pUi;
    QButtonGroup *m_pLeftNavigationGroup;
    BasicSettingPage *m_pBasicSettingPage;
    ImageCorrectionPage *m_pImageCorrectionPage;
    ZStackSettingPage *m_pZStackSettingPage;
    SampleStageWidget *m_pSampleStageWidget;
    PreviewStatusBarWidget *m_pStatusBarWidget;
};

#endif // PREVIEWPAGE_H
