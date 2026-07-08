#include "scanpage.h"

#include "CreateExperimentSubPage.h"
#include "ui_scanpage.h"

#include <QComboBox>
#include <QPoint>
#include <QPushButton>
#include <QVariant>

ScanPage::ScanPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScanPage)
    , m_pCreateExperimentSubPage(new CreateExperimentSubPage)
{
    ui->setupUi(this);

    initControls();
    initConnections();
}

ScanPage::~ScanPage()
{
    delete m_pCreateExperimentSubPage;
    delete ui;
}

void ScanPage::initControls()
{
    m_pCreateExperimentSubPage->hide();
    setExperimentActionEnabled(false);

    ui->comboPlateFormat->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate6),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate6)));
    ui->comboPlateFormat->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate12),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate12)));
    ui->comboPlateFormat->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate24),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate24)));
    ui->comboPlateFormat->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate48),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate48)));
    ui->comboPlateFormat->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate96),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate96)));

    for (int group = 1; group <= 10; ++group)
    {
        ui->comboScanGroup->addItem(QStringLiteral("分组%1").arg(group), group);
    }

    setPlateFormat(WellPlateWidget::PlateFormat::Plate12);
    ui->comboScanGroup->setCurrentIndex(0);
}

void ScanPage::initConnections()
{
    connect(ui->buttonCreateExperiment, &QPushButton::clicked, this, [this]() {
        emit createExperimentRequested();
        showCreateExperimentPage();
    });
    connect(ui->buttonEditExperimentConfig, &QPushButton::clicked, this, [this]() {
        emit editExperimentConfigRequested();
        showCreateExperimentPage();
    });
    connect(ui->buttonSaveExperimentConfig, &QPushButton::clicked,
        this, &ScanPage::saveExperimentConfigRequested);
    connect(ui->buttonStopScan, &QPushButton::clicked,
        this, &ScanPage::stopScanRequested);
    connect(ui->buttonBrowseData, &QPushButton::clicked,
        this, &ScanPage::browseDataRequested);
    connect(ui->buttonSelectWells, &QPushButton::clicked,
        this, &ScanPage::selectWellsRequested);
    connect(ui->buttonCancelWellSelection, &QPushButton::clicked,
        this, &ScanPage::cancelWellSelectionRequested);
    connect(ui->buttonSelectFields, &QPushButton::clicked,
        this, &ScanPage::selectFieldsRequested);
    connect(ui->buttonCancelFieldSelection, &QPushButton::clicked,
        this, &ScanPage::cancelFieldSelectionRequested);
    connect(ui->buttonConfirmSelection, &QPushButton::clicked,
        this, &ScanPage::confirmSelectionRequested);

    connect(ui->comboPlateFormat, &QComboBox::currentIndexChanged, this, [this](int index) {
        const WellPlateWidget::PlateFormat format = static_cast<WellPlateWidget::PlateFormat>(
            ui->comboPlateFormat->itemData(index).toInt());
        setPlateFormat(format);
        emit plateFormatChanged(format);
    });
    connect(ui->comboScanGroup, &QComboBox::currentIndexChanged, this, [this](int index) {
        emit groupChanged(ui->comboScanGroup->itemData(index).toInt());
    });
    connect(m_pCreateExperimentSubPage, &CreateExperimentSubPage::experimentSettingsChanged,
        this, &ScanPage::applyExperimentSettings);
    connect(m_pCreateExperimentSubPage, &CreateExperimentSubPage::closeRequested,
        m_pCreateExperimentSubPage, &CreateExperimentSubPage::hide);
    connect(m_pCreateExperimentSubPage, &CreateExperimentSubPage::choosePlateFieldsRequested,
        this, &ScanPage::selectFieldsRequested);
}

void ScanPage::showCreateExperimentPage()
{
    m_pCreateExperimentSubPage->resetToFirstPage();
    setExperimentActionEnabled(true);

    m_pCreateExperimentSubPage->resize(m_pCreateExperimentSubPage->minimumSize());
    const QPoint globalCenter = mapToGlobal(rect().center());
    m_pCreateExperimentSubPage->move(globalCenter.x() - m_pCreateExperimentSubPage->width() / 2,
        globalCenter.y() - m_pCreateExperimentSubPage->height() / 2);
    m_pCreateExperimentSubPage->show();
    m_pCreateExperimentSubPage->activateWindow();
    m_pCreateExperimentSubPage->raise();
}

void ScanPage::setExperimentActionEnabled(bool enabled)
{
    ui->buttonEditExperimentConfig->setEnabled(enabled);
    ui->buttonSaveExperimentConfig->setEnabled(enabled);
    ui->buttonStopScan->setEnabled(enabled);
    ui->buttonBrowseData->setEnabled(enabled);
}

void ScanPage::applyExperimentSettings(const CreateExperimentSettings &settings)
{
    ui->lineEditExperimentName->setText(settings.experimentName.isEmpty()
            ? QStringLiteral("-")
            : settings.experimentName);
    ui->lineEditExperimentNumber->setText(settings.experimentNumber.isEmpty()
            ? QStringLiteral("-")
            : settings.experimentNumber);
    ui->lineEditExperimentDate->setText(settings.experimentDate.isEmpty()
            ? QStringLiteral("-")
            : settings.experimentDate);
    ui->lineEditExperimentType->setText(settings.experimentType.isEmpty()
            ? QStringLiteral("-")
            : settings.experimentType);
    ui->lineEditScanChannel->setText(settings.scanChannel.isEmpty()
            ? QStringLiteral("-")
            : settings.scanChannel);
    ui->lineEditObjective->setText(settings.objective.isEmpty()
            ? QStringLiteral("-")
            : settings.objective);
    ui->lineEditScanType->setText(settings.scanType.isEmpty()
            ? QStringLiteral("-")
            : settings.scanType);
    ui->lineEditZStack->setText(settings.zStack.isEmpty()
            ? QStringLiteral("-")
            : settings.zStack);
    ui->lineEditIntervalTime->setText(settings.intervalTime.isEmpty()
            ? QStringLiteral("-")
            : settings.intervalTime);
    ui->lineEditLoopCount->setText(settings.loopCount.isEmpty()
            ? QStringLiteral("-")
            : settings.loopCount);
    ui->lineEditFieldStitching->setText(settings.fieldStitching.isEmpty()
            ? QStringLiteral("-")
            : settings.fieldStitching);
    ui->lineEditFieldOverlap->setText(settings.fieldOverlap.isEmpty()
            ? QStringLiteral("-")
            : settings.fieldOverlap);
    ui->lineEditFocusInterval->setText(settings.focusInterval.isEmpty()
            ? QStringLiteral("-")
            : settings.focusInterval);
    ui->lineEditFocusChannel->setText(settings.focusChannel.isEmpty()
            ? QStringLiteral("-")
            : settings.focusChannel);
}

void ScanPage::setPlateFormat(WellPlateWidget::PlateFormat format)
{
    ui->wellPlateWidget->setPlateFormat(format);
    ui->fieldViewWidget->setPlateFormat(format);

    const int target = static_cast<int>(format);
    for (int index = 0; index < ui->comboPlateFormat->count(); ++index)
    {
        if (ui->comboPlateFormat->itemData(index).toInt() == target)
        {
            ui->comboPlateFormat->setCurrentIndex(index);
            return;
        }
    }
}

QString ScanPage::plateFormatText(WellPlateWidget::PlateFormat format)
{
    switch (format)
    {
    case WellPlateWidget::PlateFormat::Plate6:
        return QStringLiteral("6孔板");
    case WellPlateWidget::PlateFormat::Plate12:
        return QStringLiteral("12孔板");
    case WellPlateWidget::PlateFormat::Plate24:
        return QStringLiteral("24孔板");
    case WellPlateWidget::PlateFormat::Plate48:
        return QStringLiteral("48孔板");
    case WellPlateWidget::PlateFormat::Plate96:
        return QStringLiteral("96孔板");
    }

    return QStringLiteral("12孔板");
}
