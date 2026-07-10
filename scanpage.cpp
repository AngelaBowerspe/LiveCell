#include "scanpage.h"

#include "ui_scanpage.h"

#include <algorithm>

#include <QColor>
#include <QComboBox>
#include <QDateTime>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTimer>
#include <QVariant>

ScanPage::ScanPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScanPage)
    , m_pCreateExperimentSubPage(new CreateExperimentSubPage(this))
    , m_pMockScanTimer(new QTimer(this))
    , m_bPlateFieldSelectionEnabled(false)
    , m_bWellSelectionMode(false)
    , m_bFieldSelectionMode(false)
    , m_bScanning(false)
    , m_bExperimentConfigured(false)
    , m_currentPreviewWell()
    , m_mockScanPlan()
    , m_nMockScanIndex(0)
    , m_selectedFieldsByWell()
    , m_selectedGroupByWell()
    , m_currentScanningTarget()
    , m_nCurrentGroupIndex(1)
{
    ui->setupUi(this);
    m_pCreateExperimentSubPage->hide();
    m_pMockScanTimer->setInterval(5000);

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
    m_nCurrentGroupIndex = ui->comboScanGroup->currentData().toInt();
    updateGroupColorButton();
    ui->buttonStopScan->setText(QStringLiteral("开始扫描"));
    setExperimentActionEnabled(false);
    updatePlateFieldControls();
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
        this, &ScanPage::toggleScan);
    connect(ui->buttonBrowseData, &QPushButton::clicked,
        this, &ScanPage::browseDataRequested);
    connect(ui->buttonSelectWells, &QPushButton::clicked,
        this, &ScanPage::handleSelectWellsButtonClicked);
    connect(ui->buttonCancelWellSelection, &QPushButton::clicked,
        this, &ScanPage::cancelWellSelection);
    connect(ui->buttonSelectFields, &QPushButton::clicked,
        this, &ScanPage::handleSelectFieldsButtonClicked);
    connect(ui->buttonCancelFieldSelection, &QPushButton::clicked,
        this, &ScanPage::cancelFieldSelection);
    connect(ui->buttonConfirmSelection, &QPushButton::clicked,
        this, &ScanPage::finishPlateFieldSelection);

    connect(ui->comboPlateFormat, &QComboBox::currentIndexChanged, this, [this](int index) {
        const WellPlateWidget::PlateFormat format = static_cast<WellPlateWidget::PlateFormat>(
            ui->comboPlateFormat->itemData(index).toInt());
        setPlateFormat(format);
        emit plateFormatChanged(format);
    });
    connect(ui->comboScanGroup, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (m_bPlateFieldSelectionEnabled)
        {
            if (m_bFieldSelectionMode)
            {
                discardUnconfirmedFieldSelection();
            }
            if (m_bWellSelectionMode && !ui->wellPlateWidget->selectedWells().isEmpty())
            {
                QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先点击“选择孔区”确认当前孔位，或点击“取消选择”清除当前孔位。"));
                const QSignalBlocker blocker(ui->comboScanGroup);
                ui->comboScanGroup->setCurrentIndex(ui->comboScanGroup->findData(m_nCurrentGroupIndex));
                return;
            }

            const QString missingWell = firstWellWithoutFields();
            if (!missingWell.isEmpty())
            {
                QMessageBox::warning(this, QStringLiteral("提示"),
                    QStringLiteral("请先为孔位 %1 选择视野，再切换分组。").arg(missingWell));
                const QSignalBlocker blocker(ui->comboScanGroup);
                ui->comboScanGroup->setCurrentIndex(ui->comboScanGroup->findData(m_nCurrentGroupIndex));
                return;
            }
        }

        m_nCurrentGroupIndex = ui->comboScanGroup->itemData(index).toInt();
        updateGroupColorButton();
        emit groupChanged(m_nCurrentGroupIndex);
    });
    connect(ui->wellPlateWidget, &WellPlateWidget::wellClicked,
        this, &ScanPage::handleWellClicked);
    connect(ui->wellPlateWidget, &WellPlateWidget::wellSelectionChanged,
        this, &ScanPage::handleWellSelectionChanged);
    connect(m_pMockScanTimer, &QTimer::timeout,
        this, &ScanPage::runNextMockScanStep);

    connect(m_pCreateExperimentSubPage, &CreateExperimentSubPage::experimentPageAccepted,
        this, &ScanPage::applyAcceptedExperimentPage);
    connect(m_pCreateExperimentSubPage, &CreateExperimentSubPage::experimentFinished,
        this, &ScanPage::completeExperimentConfiguration);
    connect(m_pCreateExperimentSubPage, &CreateExperimentSubPage::choosePlateFieldsRequested,
        this, &ScanPage::enablePlateFieldSelection);
}

void ScanPage::showCreateExperimentPage()
{
    emit createExperimentRequested();

    m_bExperimentConfigured = false;
    m_bPlateFieldSelectionEnabled = false;
    m_bWellSelectionMode = false;
    m_bFieldSelectionMode = false;
    m_currentPreviewWell.clear();
    m_selectedFieldsByWell.clear();
    m_selectedGroupByWell.clear();
    ui->wellPlateWidget->clearAll();
    ui->fieldViewWidget->clearAll();
    m_pCreateExperimentSubPage->setPlateFieldSelectionSummary(QString(), QString(), QString(), QString());
    setExperimentActionEnabled(false);
    updatePlateFieldControls();
    m_pCreateExperimentSubPage->resetToFirstPage();
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

void ScanPage::completeExperimentConfiguration()
{
    m_bExperimentConfigured = true;
    setExperimentActionEnabled(true);
}

void ScanPage::enablePlateFieldSelection()
{
    m_bPlateFieldSelectionEnabled = true;
    m_pCreateExperimentSubPage->hide();
    beginWellSelection();
}

void ScanPage::handleSelectWellsButtonClicked()
{
    if (!m_bPlateFieldSelectionEnabled)
    {
        return;
    }
    if (m_bFieldSelectionMode)
    {
        discardUnconfirmedFieldSelection();
    }

    if (!ui->wellPlateWidget->selectedWells().isEmpty())
    {
        confirmWellSelection();
        return;
    }

    QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先用鼠标选择孔区。"));
}

void ScanPage::handleSelectFieldsButtonClicked()
{
    if (!m_bPlateFieldSelectionEnabled)
    {
        return;
    }
    if (m_bWellSelectionMode)
    {
        if (!ui->wellPlateWidget->selectedWells().isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先点击“选择孔区”确认当前孔位。"));
            return;
        }
    }
    if (!m_bFieldSelectionMode)
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先点击一个已确认孔位。"));
        return;
    }

    confirmFieldSelection();
}

void ScanPage::finishPlateFieldSelection()
{
    if (!m_bPlateFieldSelectionEnabled)
    {
        return;
    }

    if (m_bWellSelectionMode)
    {
        if (!ui->wellPlateWidget->selectedWells().isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先点击“选择孔区”确认当前孔位。"));
            return;
        }
    }
    if (m_bFieldSelectionMode)
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先点击“选择视野”确认当前孔位视野。"));
        return;
    }

    if (groupedWells().isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请至少选择一个孔位。"));
        beginWellSelection();
        return;
    }

    const QString missingWell = firstWellWithoutFields();
    if (!missingWell.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"),
            QStringLiteral("请先为孔位 %1 选择视野。").arg(missingWell));
        return;
    }

    m_bPlateFieldSelectionEnabled = false;
    updateCreateExperimentPlateFieldSummary();
    updatePlateFieldControls();
    showCreateExperimentSelectionPage();
    emit confirmSelectionRequested();
}

void ScanPage::beginWellSelection()
{
    if (!m_bPlateFieldSelectionEnabled)
    {
        return;
    }

    ui->wellPlateWidget->setGroupColor(currentGroupColor());
    ui->wellPlateWidget->setSelectionEnabled(true);
    ui->fieldViewWidget->setSelectionEnabled(false);
    ui->fieldViewWidget->clearAll();
    m_bWellSelectionMode = true;
    m_bFieldSelectionMode = false;
    m_currentPreviewWell.clear();
    ui->wellPlateWidget->setActiveWell(QString());
    updatePlateFieldControls();
    emit selectWellsRequested();
}

void ScanPage::cancelWellSelection()
{
    const QStringList selectedWells = ui->wellPlateWidget->selectedWells();
    if (!selectedWells.isEmpty())
    {
        ui->wellPlateWidget->clearSelected();
        refreshPlateSelectionFromModel();
    }

    updateCreateExperimentPlateFieldSummary();
    updatePlateFieldControls();
    emit cancelWellSelectionRequested();
}

void ScanPage::confirmWellSelection()
{
    const QStringList selectedWells = ui->wellPlateWidget->selectedWells();
    if (selectedWells.isEmpty())
    {
        return;
    }

    const int groupIndex = ui->comboScanGroup->currentData().toInt();
    for (const QString &well : selectedWells)
    {
        m_selectedGroupByWell.insert(well, groupIndex);
    }

    ui->wellPlateWidget->confirmSelectedAsGroup(currentGroupColor());
    ui->wellPlateWidget->setSelectionEnabled(true);
    ui->wellPlateWidget->setActiveWell(QString());
    ui->fieldViewWidget->clearAll();
    m_currentPreviewWell.clear();
    m_bWellSelectionMode = true;
    updateCreateExperimentPlateFieldSummary();
    updatePlateFieldControls();
}

void ScanPage::beginFieldSelectionForWell(const QString &well)
{
    if (well.trimmed().isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先选择一个已分组孔位。"));
        return;
    }

    const WellPlateWidget::WellState state = ui->wellPlateWidget->wellState(well);
    if (state != WellPlateWidget::WellState::Grouped
        && state != WellPlateWidget::WellState::Completed
        && state != WellPlateWidget::WellState::Scanning)
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先选择一个已分组孔位。"));
        return;
    }

    m_currentPreviewWell = well;
    ui->wellPlateWidget->setActiveWell(well);
    restoreFieldsForActiveWell();
    ui->fieldViewWidget->setSelectionEnabled(true);
    m_bFieldSelectionMode = true;
    m_bWellSelectionMode = false;
    updatePlateFieldControls();
    emit selectFieldsRequested();
}

void ScanPage::cancelFieldSelection()
{
    if (!m_currentPreviewWell.isEmpty())
    {
        m_selectedFieldsByWell.remove(m_currentPreviewWell);
    }
    ui->fieldViewWidget->clearState(FieldViewWidget::FieldState::Selected);
    updateCreateExperimentPlateFieldSummary();
    updatePlateFieldControls();
    emit cancelFieldSelectionRequested();
}

void ScanPage::confirmFieldSelection()
{
    if (m_currentPreviewWell.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先选择一个已分组孔位。"));
        return;
    }

    const QSet<int> fields = ui->fieldViewWidget->selectedFieldIndexes();
    if (fields.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请至少选择一个视野。"));
        return;
    }

    m_selectedFieldsByWell.insert(m_currentPreviewWell, fields);
    ui->fieldViewWidget->setSelectionEnabled(false);
    m_bFieldSelectionMode = false;
    updateCreateExperimentPlateFieldSummary();
    beginWellSelection();
    updatePlateFieldControls();
}

void ScanPage::handleWellClicked(const QString &well)
{
    if (!m_bPlateFieldSelectionEnabled)
    {
        return;
    }

    if (!ui->wellPlateWidget->selectedWells().isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"),
            QStringLiteral("请先点击“选择孔区”确认当前孔位，或点击“取消选择”清除当前孔位。"));
        return;
    }

    if (m_bFieldSelectionMode)
    {
        discardUnconfirmedFieldSelection();
    }

    const WellPlateWidget::WellState state = ui->wellPlateWidget->wellState(well);
    if (state != WellPlateWidget::WellState::Grouped
        && state != WellPlateWidget::WellState::Completed)
    {
        return;
    }

    beginFieldSelectionForWell(well);
}

void ScanPage::handleWellSelectionChanged()
{
    if (!m_bPlateFieldSelectionEnabled || !m_bFieldSelectionMode)
    {
        return;
    }

    if (ui->wellPlateWidget->selectedWells().isEmpty())
    {
        return;
    }

    discardUnconfirmedFieldSelection();
    m_currentPreviewWell.clear();
    m_bWellSelectionMode = true;
    ui->wellPlateWidget->setActiveWell(QString());
    updatePlateFieldControls();
}

void ScanPage::toggleScan()
{
    if (!m_bExperimentConfigured)
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先完成实验创建。"));
        return;
    }

    if (m_bScanning)
    {
        stopMockScan();
        emit stopScanRequested();
        return;
    }

    const QString missingWell = firstWellWithoutFields();
    if (!missingWell.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"),
            QStringLiteral("请先为孔位 %1 选择视野。").arg(missingWell));
        return;
    }

    if (!buildMockScanPlan())
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先为已选孔区选择视野。"));
        return;
    }

    m_bScanning = true;
    m_nMockScanIndex = 0;
    m_currentScanningTarget = QPair<QString, int>();
    ui->buttonStopScan->setText(QStringLiteral("停止扫描"));
    runNextMockScanStep();
    m_pMockScanTimer->start();
}

void ScanPage::runNextMockScanStep()
{
    if (!m_currentScanningTarget.first.isEmpty())
    {
        ui->fieldViewWidget->setFieldState(m_currentScanningTarget.second,
            FieldViewWidget::FieldState::Completed);
        ui->wellPlateWidget->setWellState(m_currentScanningTarget.first,
            WellPlateWidget::WellState::Completed);
    }

    if (m_nMockScanIndex >= m_mockScanPlan.count())
    {
        stopMockScan();
        return;
    }

    m_currentScanningTarget = m_mockScanPlan.at(m_nMockScanIndex);
    ++m_nMockScanIndex;

    m_currentPreviewWell = m_currentScanningTarget.first;
    ui->wellPlateWidget->setActiveWell(m_currentPreviewWell);
    restoreFieldsForActiveWell();
    ui->wellPlateWidget->setWellState(m_currentScanningTarget.first,
        WellPlateWidget::WellState::Scanning);
    ui->fieldViewWidget->setFieldState(m_currentScanningTarget.second,
        FieldViewWidget::FieldState::Scanning);
}

void ScanPage::stopMockScan()
{
    m_pMockScanTimer->stop();
    m_bScanning = false;
    ui->buttonStopScan->setText(QStringLiteral("开始扫描"));
    updatePlateFieldControls();
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

void ScanPage::updatePlateFieldControls()
{
    ui->wellPlateWidget->setEnabled(m_bPlateFieldSelectionEnabled && !m_bScanning);
    ui->fieldViewWidget->setEnabled(m_bPlateFieldSelectionEnabled && m_bFieldSelectionMode && !m_bScanning);

    ui->buttonSelectWells->setEnabled(m_bPlateFieldSelectionEnabled && !m_bScanning);
    ui->buttonCancelWellSelection->setEnabled(m_bPlateFieldSelectionEnabled && !m_bScanning);
    ui->buttonSelectFields->setEnabled(m_bPlateFieldSelectionEnabled && !m_bScanning && m_bFieldSelectionMode);
    ui->buttonCancelFieldSelection->setEnabled(m_bPlateFieldSelectionEnabled && !m_bScanning && m_bFieldSelectionMode);
    ui->buttonConfirmSelection->setEnabled(m_bPlateFieldSelectionEnabled && !m_bScanning);
}

void ScanPage::setExperimentActionEnabled(bool enabled)
{
    ui->buttonEditExperimentConfig->setEnabled(enabled);
    ui->buttonSaveExperimentConfig->setEnabled(enabled);
    ui->buttonStopScan->setEnabled(enabled);
    ui->buttonBrowseData->setEnabled(enabled);
}

void ScanPage::setPlateFormat(WellPlateWidget::PlateFormat format)
{
    ui->wellPlateWidget->setPlateFormat(format);
    ui->fieldViewWidget->setPlateFormat(format);
    m_currentPreviewWell.clear();
    m_selectedFieldsByWell.clear();
    m_selectedGroupByWell.clear();
    m_mockScanPlan.clear();
    updatePlateFieldControls();

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

bool ScanPage::buildMockScanPlan()
{
    m_mockScanPlan.clear();
    for (const QString &well : groupedWells())
    {
        QList<int> fields = m_selectedFieldsByWell.value(well).values();
        std::sort(fields.begin(), fields.end());
        for (const int field : fields)
        {
            m_mockScanPlan.append(qMakePair(well, field));
        }
    }

    return !m_mockScanPlan.isEmpty();
}

void ScanPage::discardUnconfirmedFieldSelection()
{
    ui->fieldViewWidget->clearState(FieldViewWidget::FieldState::Selected);
    ui->fieldViewWidget->setSelectionEnabled(false);
    m_bFieldSelectionMode = false;
}

void ScanPage::restoreFieldsForActiveWell()
{
    ui->fieldViewWidget->clearAll();
    if (m_currentPreviewWell.isEmpty())
    {
        return;
    }

    ui->fieldViewWidget->setFieldStates(m_selectedFieldsByWell.value(m_currentPreviewWell),
        FieldViewWidget::FieldState::Selected);
}

void ScanPage::showCreateExperimentSelectionPage()
{
    m_pCreateExperimentSubPage->showCenteredIn(this);
}

void ScanPage::updateCreateExperimentPlateFieldSummary()
{
    const QStringList wells = selectedWellsInPlateOrder();
    int fieldCount = 0;
    for (const QString &well : wells)
    {
        fieldCount += m_selectedFieldsByWell.value(well).count();
    }

    m_pCreateExperimentSubPage->setPlateFieldSelectionSummary(
        ui->comboPlateFormat->currentText(),
        wells.join(QStringLiteral(", ")),
        selectedGroupsText(),
        fieldCount > 0 ? QStringLiteral("%1个").arg(fieldCount) : QString());
}

void ScanPage::refreshPlateSelectionFromModel()
{
    QMap<int, QStringList> wellsByGroup;
    for (auto it = m_selectedGroupByWell.cbegin(); it != m_selectedGroupByWell.cend(); ++it)
    {
        wellsByGroup[it.value()].append(it.key());
    }

    for (auto it = wellsByGroup.cbegin(); it != wellsByGroup.cend(); ++it)
    {
        ui->wellPlateWidget->setGroupedWells(it.value(), groupColor(it.key()));
    }
}

QStringList ScanPage::groupedWells() const
{
    return selectedWellsInPlateOrder();
}

QStringList ScanPage::selectedWellsInPlateOrder() const
{
    QStringList wells;
    for (int row = 0; row < ui->wellPlateWidget->rowCount(); ++row)
    {
        for (int column = 0; column < ui->wellPlateWidget->columnCount(); ++column)
        {
            const QString well = ui->wellPlateWidget->wellName(row, column);
            if (m_selectedGroupByWell.contains(well))
            {
                wells.append(well);
            }
        }
    }

    return wells;
}

QString ScanPage::selectedGroupsText() const
{
    QList<int> groupIndexes;
    for (auto it = m_selectedGroupByWell.cbegin(); it != m_selectedGroupByWell.cend(); ++it)
    {
        const int groupIndex = it.value();
        if (!groupIndexes.contains(groupIndex))
        {
            groupIndexes.append(groupIndex);
        }
    }
    std::sort(groupIndexes.begin(), groupIndexes.end());

    QStringList groupTexts;
    for (const int groupIndex : groupIndexes)
    {
        groupTexts.append(QStringLiteral("分组%1").arg(groupIndex));
    }

    return groupTexts.join(QStringLiteral(", "));
}

QString ScanPage::firstWellWithoutFields() const
{
    for (const QString &well : groupedWells())
    {
        if (m_selectedFieldsByWell.value(well).isEmpty())
        {
            return well;
        }
    }

    return QString();
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
        return QColor(118, 212, 122, 125);
    case 2:
        return QColor(176, 150, 245, 125);
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
