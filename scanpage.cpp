#include "scanpage.h"

#include "ui_scanpage.h"

#include <QColor>
#include <QComboBox>
#include <QDateTime>
#include <QLineEdit>
#include <QPushButton>
#include <QVariant>

ScanPage::ScanPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScanPage)
    , m_pCreateExperimentSubPage(new CreateExperimentSubPage(this))
{
    ui->setupUi(this);
    m_pCreateExperimentSubPage->hide();

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
    updateGroupColorButton();
    setExperimentActionEnabled(false);
}

void ScanPage::initConnections()
{
    connect(ui->buttonCreateExperiment, &QPushButton::clicked,
        this, &ScanPage::showCreateExperimentPage);
    connect(ui->buttonEditExperimentConfig, &QPushButton::clicked,
        this, &ScanPage::editExperimentConfigRequested);
    connect(ui->buttonSaveExperimentConfig, &QPushButton::clicked,
        this, &ScanPage::saveExperimentConfigRequested);
    connect(ui->buttonStopScan, &QPushButton::clicked,
        this, &ScanPage::stopScanRequested);
    connect(ui->buttonBrowseData, &QPushButton::clicked,
        this, &ScanPage::browseDataRequested);
    connect(ui->buttonSelectWells, &QPushButton::clicked,
        this, &ScanPage::beginWellSelection);
    connect(ui->buttonCancelWellSelection, &QPushButton::clicked,
        this, &ScanPage::cancelWellSelection);
    connect(ui->buttonSelectFields, &QPushButton::clicked,
        this, &ScanPage::selectFieldsRequested);
    connect(ui->buttonCancelFieldSelection, &QPushButton::clicked,
        this, &ScanPage::cancelFieldSelectionRequested);
    connect(ui->buttonConfirmSelection, &QPushButton::clicked,
        this, &ScanPage::confirmWellSelection);

    connect(ui->comboPlateFormat, &QComboBox::currentIndexChanged, this, [this](int index) {
        const WellPlateWidget::PlateFormat format = static_cast<WellPlateWidget::PlateFormat>(
            ui->comboPlateFormat->itemData(index).toInt());
        setPlateFormat(format);
        emit plateFormatChanged(format);
    });
    connect(ui->comboScanGroup, &QComboBox::currentIndexChanged, this, [this](int index) {
        updateGroupColorButton();
        emit groupChanged(ui->comboScanGroup->itemData(index).toInt());
    });

    connect(m_pCreateExperimentSubPage, &CreateExperimentSubPage::experimentPageAccepted,
        this, &ScanPage::applyAcceptedExperimentPage);
    connect(m_pCreateExperimentSubPage, &CreateExperimentSubPage::choosePlateFieldsRequested,
        this, &ScanPage::selectFieldsRequested);
}

void ScanPage::showCreateExperimentPage()
{
    emit createExperimentRequested();

    setExperimentActionEnabled(true);
    m_pCreateExperimentSubPage->showCenteredIn(this);
}

void ScanPage::applyAcceptedExperimentPage(CreateExperimentSubPage::AcceptedPage page)
{
    switch (page)
    {
    case CreateExperimentSubPage::AcceptedPage::CreateMode:
    {
        const QString experimentName = m_pCreateExperimentSubPage->experimentName();
        ui->lineEditExperimentName->setText(experimentName.isEmpty() ? QStringLiteral("-") : experimentName);
        if (ui->lineEditExperimentNumber->text().trimmed().isEmpty()
            || ui->lineEditExperimentNumber->text().trimmed() == QStringLiteral("-"))
        {
            ui->lineEditExperimentNumber->setText(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMddhhmmss")));
        }
        if (ui->lineEditExperimentDate->text().trimmed().isEmpty()
            || ui->lineEditExperimentDate->text().trimmed() == QStringLiteral("-"))
        {
            ui->lineEditExperimentDate->setText(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")));
        }
        break;
    }
    case CreateExperimentSubPage::AcceptedPage::ExperimentType:
        ui->lineEditExperimentType->setText(m_pCreateExperimentSubPage->experimentType());
        ui->lineEditScanChannel->setText(m_pCreateExperimentSubPage->scanChannelText());
        break;
    case CreateExperimentSubPage::AcceptedPage::DelaySetting:
        ui->lineEditScanType->setText(m_pCreateExperimentSubPage->scanModeText());
        ui->lineEditIntervalTime->setText(m_pCreateExperimentSubPage->intervalTimeText());
        ui->lineEditLoopCount->setText(m_pCreateExperimentSubPage->loopCountText());
        break;
    case CreateExperimentSubPage::AcceptedPage::PlateField:
        break;
    case CreateExperimentSubPage::AcceptedPage::ScanParameter:
        ui->lineEditFieldOverlap->setText(m_pCreateExperimentSubPage->fieldOverlapText());
        ui->lineEditFocusInterval->setText(m_pCreateExperimentSubPage->focusIntervalText());
        ui->lineEditFocusChannel->setText(m_pCreateExperimentSubPage->focusChannelText());
        break;
    case CreateExperimentSubPage::AcceptedPage::Summary:
        applyAcceptedExperimentPage(CreateExperimentSubPage::AcceptedPage::CreateMode);
        applyAcceptedExperimentPage(CreateExperimentSubPage::AcceptedPage::ExperimentType);
        applyAcceptedExperimentPage(CreateExperimentSubPage::AcceptedPage::DelaySetting);
        applyAcceptedExperimentPage(CreateExperimentSubPage::AcceptedPage::ScanParameter);
        break;
    }
}

void ScanPage::beginWellSelection()
{
    ui->wellPlateWidget->setGroupColor(currentGroupColor());
    ui->wellPlateWidget->setSelectionEnabled(true);
    emit selectWellsRequested();
}

void ScanPage::cancelWellSelection()
{
    ui->wellPlateWidget->clearSelected();
    ui->wellPlateWidget->setSelectionEnabled(false);
    emit cancelWellSelectionRequested();
}

void ScanPage::confirmWellSelection()
{
    ui->wellPlateWidget->confirmSelectedAsGroup(currentGroupColor());
    ui->wellPlateWidget->setSelectionEnabled(false);
    emit confirmSelectionRequested();
}

void ScanPage::updateGroupColorButton()
{
    const QColor color = currentGroupColor();
    ui->buttonGroupColor->setStyleSheet(QStringLiteral(
        "QPushButton#buttonGroupColor { background-color: rgba(%1, %2, %3, %4); border: 1px solid rgb(80, 130, 210); }")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alpha()));
    ui->wellPlateWidget->setGroupColor(color);
}

void ScanPage::setExperimentActionEnabled(bool enabled)
{
    ui->buttonEditExperimentConfig->setEnabled(enabled);
    ui->buttonSaveExperimentConfig->setEnabled(enabled);
    ui->buttonStopScan->setEnabled(false);
    ui->buttonBrowseData->setEnabled(enabled);
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

QColor ScanPage::currentGroupColor() const
{
    return groupColor(ui->comboScanGroup->currentData().toInt());
}

QColor ScanPage::groupColor(int groupIndex)
{
    switch (groupIndex)
    {
    case 1:
        return QColor(112, 166, 238, 125);
    case 2:
        return QColor(118, 212, 122, 125);
    case 3:
        return QColor(255, 209, 84, 130);
    case 4:
        return QColor(226, 132, 230, 125);
    case 5:
        return QColor(255, 147, 103, 125);
    case 6:
        return QColor(94, 204, 212, 125);
    case 7:
        return QColor(176, 150, 245, 125);
    case 8:
        return QColor(184, 206, 89, 125);
    case 9:
        return QColor(238, 140, 168, 125);
    case 10:
        return QColor(142, 186, 224, 125);
    default:
        return QColor(112, 166, 238, 125);
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
