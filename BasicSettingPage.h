#ifndef BASICSETTINGPAGE_H
#define BASICSETTINGPAGE_H

#include "models/CaptureSettings.h"
#include "models/StageControlSettings.h"

#include <QButtonGroup>
#include <QLineEdit>
#include <QSlider>
#include <QWidget>

namespace Ui {
class BasicSettingPage;
}

class BasicSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit BasicSettingPage(QWidget *parent = nullptr);
    ~BasicSettingPage();

    CaptureSettings captureSettings() const;
    StageControlSettings stageControlSettings() const;

    void setRecording(bool recording);
    void setExposureControlsEnabled(bool enabled);

signals:
    void captureRequested();
    void recordingToggled(bool checked);
    void captureSettingsChanged();
    void stageControlChanged();
    void autoStageRequested();

private:
    void initButtonGroups();
    void initConnections();
    void initValidators();
    void syncSliderAndLineEdit(QSlider *pSlider, QLineEdit *pLineEdit, double scale, int decimals);
    int lineEditIntValue(const QLineEdit *pLineEdit) const;
    double lineEditDoubleValue(const QLineEdit *pLineEdit) const;
    QVector<CaptureChannel> checkedChannels() const;
    QVector<CaptureChannel> checkedMultiChannels() const;

private:
    Ui::BasicSettingPage *m_pUi;
    QButtonGroup *m_pObjectiveGroup;
    QButtonGroup *m_pChannelGroup;
};

#endif // BASICSETTINGPAGE_H
