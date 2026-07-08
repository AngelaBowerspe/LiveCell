#include "BasicSettingPage.h"

#include "ui_BasicSettingPage.h"

#include <QCheckBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QSignalBlocker>
#include <QtGlobal>

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
    settings.lightIntensity = lineEditIntValue(m_pUi->lineEditLightIntensity);
    settings.autoExposure = m_pUi->checkAutoExposure->isChecked();
    settings.exposure = lineEditDoubleValue(m_pUi->lineEditExposure);
    settings.gain = lineEditDoubleValue(m_pUi->lineEditGain);
    settings.contrast = lineEditIntValue(m_pUi->lineEditContrast);
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
    m_pUi->lineEditExposure->setEnabled(enabled);
    m_pUi->sliderGain->setEnabled(enabled);
    m_pUi->lineEditGain->setEnabled(enabled);
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
    initValidators();
    syncSliderAndLineEdit(m_pUi->sliderLightIntensity, m_pUi->lineEditLightIntensity, 1.0, 0);
    syncSliderAndLineEdit(m_pUi->sliderExposure, m_pUi->lineEditExposure, 10.0, 1);
    syncSliderAndLineEdit(m_pUi->sliderGain, m_pUi->lineEditGain, 10.0, 1);
    syncSliderAndLineEdit(m_pUi->sliderContrast, m_pUi->lineEditContrast, 1.0, 0);

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

void BasicSettingPage::initValidators()
{
    m_pUi->lineEditLightIntensity->setValidator(new QIntValidator(1, 100, this));
    m_pUi->lineEditExposure->setValidator(new QDoubleValidator(0.0, 500.0, 1, this));
    m_pUi->lineEditGain->setValidator(new QDoubleValidator(1.0, 100.0, 1, this));
    m_pUi->lineEditContrast->setValidator(new QIntValidator(-100, 100, this));
}

void BasicSettingPage::syncSliderAndLineEdit(QSlider *pSlider,
    QLineEdit *pLineEdit,
    double scale,
    int decimals)
{
    auto formatValue = [scale, decimals](int value) {
        const double displayValue = value / scale;
        return decimals == 0
            ? QString::number(static_cast<int>(displayValue))
            : QString::number(displayValue, 'f', decimals);
    };

    connect(pSlider, &QSlider::valueChanged, this, [this, pLineEdit, formatValue](int value) {
        const QSignalBlocker blocker(pLineEdit);
        pLineEdit->setText(formatValue(value));
        emit captureSettingsChanged();
    });

    connect(pLineEdit, &QLineEdit::editingFinished, this, [this, pSlider, pLineEdit, scale, formatValue]() {
        bool ok = false;
        const double inputValue = pLineEdit->text().toDouble(&ok);
        const int oldValue = pSlider->value();
        const int newValue = ok
            ? qBound(pSlider->minimum(), qRound(inputValue * scale), pSlider->maximum())
            : oldValue;

        pSlider->setValue(newValue);
        pLineEdit->setText(formatValue(newValue));
        if (newValue == oldValue)
        {
            emit captureSettingsChanged();
        }
    });
}

int BasicSettingPage::lineEditIntValue(const QLineEdit *pLineEdit) const
{
    return pLineEdit->text().toInt();
}

double BasicSettingPage::lineEditDoubleValue(const QLineEdit *pLineEdit) const
{
    return pLineEdit->text().toDouble();
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
