#include "scanpage.h"

#include "ui_scanpage.h"

#include <QComboBox>
#include <QPushButton>
#include <QVariant>

ScanPage::ScanPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScanPage)
{
    ui->setupUi(this);

    initControls();
    initConnections();
}

ScanPage::~ScanPage()
{
    delete ui;
}

void ScanPage::initControls()
{
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
    connect(ui->buttonCreateExperiment, &QPushButton::clicked,
        this, &ScanPage::createExperimentRequested);
    connect(ui->buttonEditExperimentConfig, &QPushButton::clicked,
        this, &ScanPage::editExperimentConfigRequested);
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
