#include "BasicSettingPage.h"

#include "ui_BasicSettingPage.h"

#include <QCheckBox>
#include <QList>
#include <QPushButton>
#include <QSignalBlocker>

BasicSettingPage::BasicSettingPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::BasicSettingPage)
    , m_pObjectiveGroup(new QButtonGroup(this))
    , m_pChannelGroup(new QButtonGroup(this))
{
    m_pUi->setupUi(this);

    initButtonGroups();
    initConnections();
}

BasicSettingPage::~BasicSettingPage()
{
    delete m_pUi;
}

CaptureSettings BasicSettingPage::captureSettings() const
{
    CaptureSettings settings;
    settings.objective = m_pUi->buttonObjective20X->isChecked()
        ? ObjectiveMagnification::Objective20X
        : ObjectiveMagnification::Objective10X;
    settings.activeChannels = checkedChannels();
    settings.lightIntensity = m_pUi->spinLightIntensity->value();
    settings.autoExposure = m_pUi->checkAutoExposure->isChecked();
    settings.exposure = m_pUi->spinExposure->value();
    settings.gain = m_pUi->spinGain->value();
    settings.contrast = m_pUi->spinContrast->value();
    settings.multiChannels = checkedMultiChannels();
    return settings;
}

StageControlSettings BasicSettingPage::stageControlSettings() const
{
    StageControlSettings settings;
    settings.coarse = m_pUi->sliderCoarseStage->value();
    settings.fine = m_pUi->sliderFineStage->value();
    return settings;
}

void BasicSettingPage::setRecording(bool recording)
{
    const QSignalBlocker blocker(m_pUi->buttonRecordVideo);
    m_pUi->buttonRecordVideo->setChecked(recording);
    m_pUi->buttonRecordVideo->setText(recording ? QStringLiteral("停止录像") : QStringLiteral("录像"));
}

void BasicSettingPage::setExposureControlsEnabled(bool enabled)
{
    m_pUi->sliderExposure->setEnabled(enabled);
    m_pUi->spinExposure->setEnabled(enabled);
}

void BasicSettingPage::initButtonGroups()
{
    m_pObjectiveGroup->setExclusive(true);
    m_pObjectiveGroup->addButton(m_pUi->buttonObjective10X);
    m_pObjectiveGroup->addButton(m_pUi->buttonObjective20X);

    m_pChannelGroup->setExclusive(true);
    m_pChannelGroup->addButton(m_pUi->buttonBrightField);
    m_pChannelGroup->addButton(m_pUi->buttonBlueChannel);
    m_pChannelGroup->addButton(m_pUi->buttonGreenChannel);
    m_pChannelGroup->addButton(m_pUi->buttonRedChannel);
}

void BasicSettingPage::initConnections()
{
    syncSliderAndSpinBox(m_pUi->sliderLightIntensity, m_pUi->spinLightIntensity);
    syncSliderAndSpinBox(m_pUi->sliderExposure, m_pUi->spinExposure);
    syncSliderAndSpinBox(m_pUi->sliderGain, m_pUi->spinGain);
    syncSliderAndSpinBox(m_pUi->sliderContrast, m_pUi->spinContrast);

    connect(m_pUi->buttonCaptureImage, &QPushButton::clicked,
        this, &BasicSettingPage::captureRequested);
    connect(m_pUi->buttonRecordVideo, &QPushButton::toggled,
        this, &BasicSettingPage::recordingToggled);

    connect(m_pObjectiveGroup, &QButtonGroup::buttonClicked,
        this, &BasicSettingPage::captureSettingsChanged);

    connect(m_pChannelGroup, &QButtonGroup::buttonClicked,
        this, &BasicSettingPage::captureSettingsChanged);

    const QList<QCheckBox *> channelChecks = {
        m_pUi->checkMultiBF,
        m_pUi->checkMultiBlue,
        m_pUi->checkMultiGreen,
        m_pUi->checkMultiRed
    };
    for (QCheckBox *pCheckBox : channelChecks)
    {
        connect(pCheckBox, &QCheckBox::toggled,
            this, &BasicSettingPage::captureSettingsChanged);
    }

    connect(m_pUi->checkAutoExposure, &QCheckBox::toggled,
        this, &BasicSettingPage::captureSettingsChanged);
    connect(m_pUi->sliderCoarseStage, &QSlider::valueChanged,
        this, &BasicSettingPage::stageControlChanged);
    connect(m_pUi->sliderFineStage, &QSlider::valueChanged,
        this, &BasicSettingPage::stageControlChanged);
    connect(m_pUi->buttonAutoStage, &QPushButton::clicked,
        this, &BasicSettingPage::autoStageRequested);
}

void BasicSettingPage::syncSliderAndSpinBox(QSlider *pSlider, QSpinBox *pSpinBox)
{
    connect(pSlider, &QSlider::valueChanged, pSpinBox, &QSpinBox::setValue);
    connect(pSpinBox, qOverload<int>(&QSpinBox::valueChanged), pSlider, &QSlider::setValue);
    connect(pSpinBox, qOverload<int>(&QSpinBox::valueChanged),
        this, &BasicSettingPage::captureSettingsChanged);
}

QVector<CaptureChannel> BasicSettingPage::checkedChannels() const
{
    QVector<CaptureChannel> channels;
    if (m_pUi->buttonBrightField->isChecked())
    {
        channels.append(CaptureChannel::BrightField);
    }
    if (m_pUi->buttonBlueChannel->isChecked())
    {
        channels.append(CaptureChannel::Blue);
    }
    if (m_pUi->buttonGreenChannel->isChecked())
    {
        channels.append(CaptureChannel::Green);
    }
    if (m_pUi->buttonRedChannel->isChecked())
    {
        channels.append(CaptureChannel::Red);
    }
    return channels;
}

QVector<CaptureChannel> BasicSettingPage::checkedMultiChannels() const
{
    QVector<CaptureChannel> channels;
    if (m_pUi->checkMultiBF->isChecked())
    {
        channels.append(CaptureChannel::BrightField);
    }
    if (m_pUi->checkMultiBlue->isChecked())
    {
        channels.append(CaptureChannel::Blue);
    }
    if (m_pUi->checkMultiGreen->isChecked())
    {
        channels.append(CaptureChannel::Green);
    }
    if (m_pUi->checkMultiRed->isChecked())
    {
        channels.append(CaptureChannel::Red);
    }
    return channels;
}
