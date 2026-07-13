#include "ImageCorrectionPage.h"

#include "HistogramRangeSlider.h"
#include "ui_ImageCorrectionPage.h"

#include <QCheckBox>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalBlocker>
#include <QtGlobal>

ImageCorrectionPage::ImageCorrectionPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::ImageCorrectionPage)
{
    m_pUi->setupUi(this);

    initValidators();
    initConnections();
}

ImageCorrectionPage::~ImageCorrectionPage()
{
    delete m_pUi;
}

ImageCorrectionSettings ImageCorrectionPage::correctionSettings() const
{
    ImageCorrectionSettings settings;
    settings.flatFieldEnabled = m_pUi->checkFlatFieldEnable->isChecked();
    settings.brightnessBalanceEnabled = m_pUi->checkBrightnessBalanceEnable->isChecked();
    settings.threshold = lineEditIntValue(m_pUi->lineEditThreshold);
    settings.histogramMin = lineEditIntValue(m_pUi->lineEditHistogramMin);
    settings.histogramMax = lineEditIntValue(m_pUi->lineEditHistogramMax);
    return settings;
}

void ImageCorrectionPage::initValidators()
{
    m_pUi->lineEditThreshold->setValidator(new QIntValidator(25, 100, this));
    m_pUi->lineEditHistogramMin->setValidator(new QIntValidator(0, 255, this));
    m_pUi->lineEditHistogramMax->setValidator(new QIntValidator(0, 255, this));
}

void ImageCorrectionPage::initConnections()
{
    connect(m_pUi->checkFlatFieldEnable, &QCheckBox::toggled,
        this, &ImageCorrectionPage::correctionSettingsChanged);
    connect(m_pUi->checkBrightnessBalanceEnable, &QCheckBox::toggled,
        this, &ImageCorrectionPage::correctionSettingsChanged);
    connect(m_pUi->buttonFlatFieldCollect, &QPushButton::clicked,
        this, &ImageCorrectionPage::flatFieldCollectRequested);
    connect(m_pUi->buttonHistogramAuto, &QPushButton::clicked,
        this, &ImageCorrectionPage::histogramAutoRequested);
    connect(m_pUi->buttonCameraCalibration, &QPushButton::clicked,
        this, &ImageCorrectionPage::cameraCalibrationRequested);
    connect(m_pUi->buttonCalibration, &QPushButton::clicked,
        this, &ImageCorrectionPage::calibrationRequested);
    connect(m_pUi->buttonMeasure, &QPushButton::clicked,
        this, &ImageCorrectionPage::measurementRequested);

    connect(m_pUi->lineEditThreshold, &QLineEdit::editingFinished,
        this, &ImageCorrectionPage::correctionSettingsChanged);

    connect(m_pUi->histogramRangeSlider, &HistogramRangeSlider::valuesChanged,
        this, [this](int lowerValue, int upperValue) {
            syncHistogramRange(lowerValue, upperValue);
            emit correctionSettingsChanged();
        });

    const auto updateRangeFromLineEdit = [this]() {
        const int lowerValue = qBound(0, lineEditIntValue(m_pUi->lineEditHistogramMin), 255);
        const int upperValue = qBound(0, lineEditIntValue(m_pUi->lineEditHistogramMax), 255);
        m_pUi->histogramRangeSlider->setValues(lowerValue, upperValue);
        syncHistogramRange(m_pUi->histogramRangeSlider->lowerValue(),
            m_pUi->histogramRangeSlider->upperValue());
        emit correctionSettingsChanged();
    };

    connect(m_pUi->lineEditHistogramMin, &QLineEdit::editingFinished,
        this, updateRangeFromLineEdit);
    connect(m_pUi->lineEditHistogramMax, &QLineEdit::editingFinished,
        this, updateRangeFromLineEdit);
}

void ImageCorrectionPage::syncHistogramRange(int lowerValue, int upperValue)
{
    const QSignalBlocker minBlocker(m_pUi->lineEditHistogramMin);
    const QSignalBlocker maxBlocker(m_pUi->lineEditHistogramMax);
    m_pUi->lineEditHistogramMin->setText(QString::number(lowerValue));
    m_pUi->lineEditHistogramMax->setText(QString::number(upperValue));
}

int ImageCorrectionPage::lineEditIntValue(const QLineEdit *pLineEdit) const
{
    return pLineEdit->text().toInt();
}
