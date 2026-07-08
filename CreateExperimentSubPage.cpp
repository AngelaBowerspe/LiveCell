#include "CreateExperimentSubPage.h"

#include "ui_CreateExperimentSubPage.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QStackedWidget>
#include <QtGlobal>

CreateExperimentSubPage::CreateExperimentSubPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::CreateExperimentSubPage)
    , m_pExperimentTypeGroup(new QButtonGroup(this))
    , m_pScanModeGroup(new QButtonGroup(this))
{
    m_pUi->setupUi(this);

    setWindowFlag(Qt::Dialog, true);
    setWindowModality(Qt::ApplicationModal);
    initControls();
    initConnections();
}

CreateExperimentSubPage::~CreateExperimentSubPage()
{
    delete m_pUi;
}

CreateExperimentSettings CreateExperimentSubPage::experimentSettings() const
{
    CreateExperimentSettings settings;
    settings.experimentName = m_pUi->lineEditExperimentName->text().trimmed();
    settings.experimentNumber = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMddHHmmss"));
    settings.experimentDate = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    settings.experimentType = selectedExperimentType();
    settings.scanChannel = selectedScanChannelText();
    settings.objective = QStringLiteral("-");
    settings.scanType = m_pScanModeGroup->checkedButton() == m_pUi->buttonLoopScanMode
            ? QStringLiteral("循环扫描")
            : QStringLiteral("单次扫描");
    settings.intervalTime = QStringLiteral("%1小时%2分钟")
            .arg(m_pUi->lineEditIntervalHours->text().trimmed().isEmpty() ? QStringLiteral("0") : m_pUi->lineEditIntervalHours->text().trimmed())
            .arg(m_pUi->lineEditIntervalMinutes->text().trimmed().isEmpty() ? QStringLiteral("0") : m_pUi->lineEditIntervalMinutes->text().trimmed());
    settings.loopCount = m_pUi->lineEditCycleCount->text().trimmed().isEmpty()
            ? QStringLiteral("-")
            : m_pUi->lineEditCycleCount->text().trimmed() + QStringLiteral("次");
    settings.zStack = m_pUi->checkEnableZStack->isChecked() ? QStringLiteral("是") : QStringLiteral("否");
    settings.fieldStitching = m_pUi->checkEnableFieldStitching->isChecked() ? QStringLiteral("是") : QStringLiteral("否");
    settings.fieldOverlap = m_pUi->lineEditFieldOverlap->text().trimmed().isEmpty()
            ? QStringLiteral("-")
            : m_pUi->lineEditFieldOverlap->text().trimmed() + QStringLiteral("%");
    settings.focusInterval = m_pUi->lineEditFocusInterval->text().trimmed().isEmpty()
            ? QStringLiteral("-")
            : m_pUi->lineEditFocusInterval->text().trimmed() + QStringLiteral("视野");
    settings.focusChannel = selectedFocusChannelText();
    settings.plateType = m_pUi->lineEditSelectedPlateType->text().trimmed().isEmpty()
            ? QStringLiteral("-")
            : m_pUi->lineEditSelectedPlateType->text().trimmed();
    settings.selectedWells = m_pUi->lineEditSelectedWells->text().trimmed().isEmpty()
            ? QStringLiteral("-")
            : m_pUi->lineEditSelectedWells->text().trimmed();
    settings.selectedGroups = m_pUi->lineEditSelectedGroups->text().trimmed().isEmpty()
            ? QStringLiteral("-")
            : m_pUi->lineEditSelectedGroups->text().trimmed();
    settings.selectedFields = m_pUi->lineEditSelectedFields->text().trimmed().isEmpty()
            ? QStringLiteral("-")
            : m_pUi->lineEditSelectedFields->text().trimmed();
    return settings;
}

void CreateExperimentSubPage::resetToFirstPage()
{
    m_pUi->pageStackedWidget->setCurrentWidget(m_pUi->pageStep1);
    updatePageIndicator();
}

void CreateExperimentSubPage::initControls()
{
    setObjectName(QStringLiteral("createExperimentSubPage"));

    m_pExperimentTypeGroup->setExclusive(true);
    m_pExperimentTypeGroup->addButton(m_pUi->buttonConfluenceAnalysis);
    m_pExperimentTypeGroup->addButton(m_pUi->buttonScratchAnalysis);
    m_pExperimentTypeGroup->addButton(m_pUi->buttonTransfectionAnalysis);
    m_pExperimentTypeGroup->addButton(m_pUi->buttonCellActivityAnalysis);

    m_pUi->buttonConfluenceAnalysis->setProperty("experimentType", QStringLiteral("汇合度分析"));
    m_pUi->buttonScratchAnalysis->setProperty("experimentType", QStringLiteral("划痕分析"));
    m_pUi->buttonTransfectionAnalysis->setProperty("experimentType", QStringLiteral("转染效率分析"));
    m_pUi->buttonCellActivityAnalysis->setProperty("experimentType", QStringLiteral("细胞活性分析"));

    m_pScanModeGroup->setExclusive(true);
    m_pScanModeGroup->addButton(m_pUi->buttonLoopScanMode);
    m_pScanModeGroup->addButton(m_pUi->buttonSingleScanMode);
    m_pUi->buttonSingleScanMode->setChecked(true);

    m_pUi->buttonCreateMode->setChecked(true);
    m_pUi->checkBrightField->setChecked(true);
    m_pUi->checkBlueFluorescence->setChecked(false);
    m_pUi->checkGreenFluorescence->setChecked(false);
    m_pUi->checkRedFluorescence->setChecked(false);
    m_pUi->checkFocusBrightField->setChecked(true);

    m_pUi->lineEditIntervalHours->setText(QStringLiteral("0"));
    m_pUi->lineEditIntervalMinutes->setText(QStringLiteral("10"));
    m_pUi->lineEditCycleCount->setText(QStringLiteral("1"));
    m_pUi->lineEditFieldOverlap->setText(QStringLiteral("0.05"));
    m_pUi->lineEditFocusInterval->setText(QStringLiteral("1"));
    m_pUi->lineEditSelectedPlateType->setText(QStringLiteral("24孔板"));
    m_pUi->lineEditSelectedWells->setText(QStringLiteral("1"));
    m_pUi->lineEditSelectedGroups->setText(QStringLiteral("1"));
    m_pUi->lineEditSelectedFields->setText(QStringLiteral("1"));

    m_pUi->pageStackedWidget->setCurrentWidget(m_pUi->pageStep1);
    updatePageIndicator();
}

void CreateExperimentSubPage::initConnections()
{
    connect(m_pUi->buttonClose, &QPushButton::clicked, this, &CreateExperimentSubPage::closeRequested);
    connect(m_pUi->buttonChoosePlateFields, &QPushButton::clicked,
        this, &CreateExperimentSubPage::choosePlateFieldsRequested);

    connect(m_pUi->buttonPreviousPage, &QPushButton::clicked,
        this, &CreateExperimentSubPage::goPreviousPage);
    connect(m_pUi->buttonNextPage, &QPushButton::clicked,
        this, &CreateExperimentSubPage::goNextPage);

    connect(m_pExperimentTypeGroup, &QButtonGroup::buttonClicked,
        this, &CreateExperimentSubPage::emitExperimentSettingsChanged);
    connect(m_pScanModeGroup, &QButtonGroup::buttonClicked,
        this, &CreateExperimentSubPage::emitExperimentSettingsChanged);

    const QList<QCheckBox *> checkBoxes = {
        m_pUi->checkBrightField,
        m_pUi->checkBlueFluorescence,
        m_pUi->checkGreenFluorescence,
        m_pUi->checkRedFluorescence,
        m_pUi->checkEnableZStack,
        m_pUi->checkEnableFieldStitching,
        m_pUi->checkFocusBrightField,
        m_pUi->checkFocusBlueFluorescence,
        m_pUi->checkFocusGreenFluorescence,
        m_pUi->checkFocusRedFluorescence
    };
    for (QCheckBox *pCheckBox : checkBoxes)
    {
        connect(pCheckBox, &QCheckBox::clicked,
            this, &CreateExperimentSubPage::emitExperimentSettingsChanged);
    }

    const QList<QLineEdit *> lineEdits = {
        m_pUi->lineEditExperimentName,
        m_pUi->lineEditIntervalHours,
        m_pUi->lineEditIntervalMinutes,
        m_pUi->lineEditCycleCount,
        m_pUi->lineEditFieldOverlap,
        m_pUi->lineEditFocusInterval,
        m_pUi->lineEditSelectedPlateType,
        m_pUi->lineEditSelectedWells,
        m_pUi->lineEditSelectedGroups,
        m_pUi->lineEditSelectedFields
    };
    for (QLineEdit *pLineEdit : lineEdits)
    {
        connect(pLineEdit, &QLineEdit::editingFinished,
            this, &CreateExperimentSubPage::emitExperimentSettingsChanged);
    }
}

void CreateExperimentSubPage::goPreviousPage()
{
    const int nextIndex = qMax(0, m_pUi->pageStackedWidget->currentIndex() - 1);
    m_pUi->pageStackedWidget->setCurrentIndex(nextIndex);
    updatePageIndicator();
}

void CreateExperimentSubPage::goNextPage()
{
    if (!validateCurrentPage())
    {
        return;
    }

    const int currentIndex = m_pUi->pageStackedWidget->currentIndex();
    if (currentIndex == m_pUi->pageStackedWidget->count() - 1)
    {
        emitExperimentSettingsChanged();
        hide();
        return;
    }

    const int nextIndex = qMin(m_pUi->pageStackedWidget->count() - 1, currentIndex + 1);
    m_pUi->pageStackedWidget->setCurrentIndex(nextIndex);
    updatePageIndicator();
    emitExperimentSettingsChanged();
}

void CreateExperimentSubPage::updatePageIndicator()
{
    const QList<QLabel *> labels = {
        m_pUi->labelPageDot1,
        m_pUi->labelPageDot2,
        m_pUi->labelPageDot3,
        m_pUi->labelPageDot4,
        m_pUi->labelPageDot5,
        m_pUi->labelPageDot6
    };

    for (int index = 0; index < labels.size(); ++index)
    {
        labels.at(index)->setText(index == m_pUi->pageStackedWidget->currentIndex()
                ? QStringLiteral("●")
                : QStringLiteral("○"));
    }
}

void CreateExperimentSubPage::emitExperimentSettingsChanged()
{
    emit experimentSettingsChanged(experimentSettings());
}

bool CreateExperimentSubPage::validateCurrentPage()
{
    if (m_pUi->pageStackedWidget->currentWidget() == m_pUi->pageStep1)
    {
        return validateFirstPage();
    }
    if (m_pUi->pageStackedWidget->currentWidget() == m_pUi->pageStep2)
    {
        return validateSecondPage();
    }
    if (m_pUi->pageStackedWidget->currentWidget() == m_pUi->pageStep5)
    {
        return validateFifthPage();
    }

    return true;
}

bool CreateExperimentSubPage::validateFirstPage()
{
    if (m_pUi->lineEditExperimentName->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先输入实验方法名称。"));
        m_pUi->lineEditExperimentName->setFocus();
        return false;
    }

    return true;
}

bool CreateExperimentSubPage::validateSecondPage()
{
    if (selectedExperimentType().isEmpty() || selectedExperimentType() == QStringLiteral("-"))
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先选择实验类型。"));
        return false;
    }

    if (selectedScanChannelText().isEmpty() || selectedScanChannelText() == QStringLiteral("-"))
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请至少选择一个扫描通道。"));
        return false;
    }

    return true;
}

bool CreateExperimentSubPage::validateFifthPage()
{
    if (selectedFocusChannelText().isEmpty() || selectedFocusChannelText() == QStringLiteral("-"))
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请至少选择一个对焦通道。"));
        return false;
    }

    return true;
}

QString CreateExperimentSubPage::selectedExperimentType() const
{
    const QAbstractButton *pButton = m_pExperimentTypeGroup->checkedButton();
    if (pButton == nullptr)
    {
        return QStringLiteral("-");
    }

    return pButton->property("experimentType").toString();
}

QString CreateExperimentSubPage::selectedScanChannelText() const
{
    QStringList channels;
    if (m_pUi->checkBrightField->isChecked())
    {
        channels.append(QStringLiteral("明场"));
    }
    if (m_pUi->checkBlueFluorescence->isChecked())
    {
        channels.append(QStringLiteral("蓝荧光"));
    }
    if (m_pUi->checkGreenFluorescence->isChecked())
    {
        channels.append(QStringLiteral("绿荧光"));
    }
    if (m_pUi->checkRedFluorescence->isChecked())
    {
        channels.append(QStringLiteral("红荧光"));
    }

    return channels.isEmpty() ? QStringLiteral("-") : channels.join(QStringLiteral(" "));
}

QString CreateExperimentSubPage::selectedFocusChannelText() const
{
    QStringList channels;
    if (m_pUi->checkFocusBrightField->isChecked())
    {
        channels.append(QStringLiteral("明场"));
    }
    if (m_pUi->checkFocusBlueFluorescence->isChecked())
    {
        channels.append(QStringLiteral("蓝荧光"));
    }
    if (m_pUi->checkFocusGreenFluorescence->isChecked())
    {
        channels.append(QStringLiteral("绿荧光"));
    }
    if (m_pUi->checkFocusRedFluorescence->isChecked())
    {
        channels.append(QStringLiteral("红荧光"));
    }

    return channels.isEmpty() ? QStringLiteral("-") : channels.join(QStringLiteral(" "));
}
