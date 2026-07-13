#include "CreateExperimentSubPage.h"

#include "ui_CreateExperimentSubPage.h"

#include <QAbstractButton>
#include <QCheckBox>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPalette>
#include <QPushButton>
#include <QSize>
#include <QStackedWidget>
#include <QStringList>

namespace {

int lineEditToNonNegativeInt(const QLineEdit *lineEdit)
{
    bool ok = false;
    const int value = lineEdit->text().trimmed().toInt(&ok);
    if (!ok)
    {
        return 0;
    }

    return qMax(0, value);
}

}

CreateExperimentSubPage::CreateExperimentSubPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CreateExperimentSubPage)
    , m_pCreateModeButtonGroup(new QButtonGroup(this))
    , m_pExperimentTypeButtonGroup(new QButtonGroup(this))
    , m_pScanModeButtonGroup(new QButtonGroup(this))
    , m_bDragging(false)
{
    ui->setupUi(this);

    initControls();
    initConnections();
}

CreateExperimentSubPage::~CreateExperimentSubPage()
{
    delete ui;
}

QString CreateExperimentSubPage::experimentName() const
{
    return ui->lineEditExperimentName->text().trimmed();
}

QString CreateExperimentSubPage::experimentType() const
{
    const QString text = checkedButtonText(m_pExperimentTypeButtonGroup);
    return text.isEmpty() ? QStringLiteral("-") : text;
}

QString CreateExperimentSubPage::scanChannelText() const
{
    const QString text = checkedChannelText();
    return text.isEmpty() ? QStringLiteral("-") : text;
}

QString CreateExperimentSubPage::scanModeText() const
{
    const QString text = checkedButtonText(m_pScanModeButtonGroup);
    return text.isEmpty() ? QStringLiteral("-") : text;
}

QString CreateExperimentSubPage::intervalTimeText() const
{
    if (!isLoopScanMode())
    {
        return QStringLiteral("-");
    }

    return QStringLiteral("%1小时%2分钟")
        .arg(ui->lineEditIntervalHours->text().trimmed())
        .arg(ui->lineEditIntervalMinutes->text().trimmed());
}

QString CreateExperimentSubPage::loopCountText() const
{
    return ui->labelAutoCycleCountValue->text().trimmed();
}

QString CreateExperimentSubPage::fieldOverlapText() const
{
    return QStringLiteral("%1%").arg(ui->lineEditFieldOverlap->text().trimmed());
}

QString CreateExperimentSubPage::focusIntervalText() const
{
    return QStringLiteral("%1视野").arg(ui->lineEditFocusInterval->text().trimmed());
}

QString CreateExperimentSubPage::focusChannelText() const
{
    const QString text = checkedFocusChannelText();
    return text.isEmpty() ? QStringLiteral("-") : text;
}

void CreateExperimentSubPage::showCenteredIn(QWidget *container)
{
    if (container == nullptr)
    {
        show();
        return;
    }

    if (parentWidget() != container)
    {
        setParent(container);
    }

    centerInContainer(container);
}

void CreateExperimentSubPage::resetToFirstPage()
{
    ui->pageStackedWidget->setCurrentWidget(ui->pageStep1);
    updatePageIndicator();
}

void CreateExperimentSubPage::setPlateFieldSelectionSummary(const QString &plateType,
    const QString &selectedWells,
    const QString &selectedGroups,
    const QString &selectedFields)
{
    const bool hasSelection = !selectedWells.trimmed().isEmpty()
        && !selectedFields.trimmed().isEmpty();
    if (!hasSelection)
    {
        resetPlateFieldSelectionSummary();
        return;
    }

    ui->lineEditPage4PlateType->setText(plateType);
    ui->lineEditPage4SelectedWells->setText(selectedWells);
    ui->lineEditPage4SelectedGroups->setText(selectedGroups);
    ui->lineEditPage4SelectedFields->setText(selectedFields);
    ui->lineSummaryPlateType->setText(plateType);
    ui->lineSummarySelectedWells->setText(selectedWells);
    ui->lineSummarySelectedGroups->setText(selectedGroups);
    ui->lineSummarySelectedFields->setText(selectedFields);
    ui->page4StateStackedWidget->setCurrentWidget(ui->page4StateSummary);
}

bool CreateExperimentSubPage::eventFilter(QObject *watched, QEvent *event)
{
    if ((watched == ui->titleBar || watched == ui->labelWindowTitle) && parentWidget() != nullptr)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(event);
            if (pMouseEvent->button() == Qt::LeftButton)
            {
                m_bDragging = true;
                m_dragOffset = mapFromGlobal(pMouseEvent->globalPosition().toPoint());
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(event);
            if (m_bDragging && (pMouseEvent->buttons() & Qt::LeftButton))
            {
                const QPoint globalTopLeft = pMouseEvent->globalPosition().toPoint() - m_dragOffset;
                QPoint nextPosition = parentWidget()->mapFromGlobal(globalTopLeft);
                const QRect bounds = parentWidget()->rect();

                const int maxX = qMax(bounds.left(), bounds.right() - width());
                const int maxY = qMax(bounds.top(), bounds.bottom() - height());
                nextPosition.setX(qBound(bounds.left(), nextPosition.x(), maxX));
                nextPosition.setY(qBound(bounds.top(), nextPosition.y(), maxY));
                move(nextPosition);
                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(event);
            if (pMouseEvent->button() == Qt::LeftButton)
            {
                m_bDragging = false;
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void CreateExperimentSubPage::initControls()
{
    setObjectName(QStringLiteral("createExperimentSubPage"));
    setWindowTitle(QStringLiteral("创建实验"));
    setAutoFillBackground(true);

    QPalette widgetPalette = palette();
    widgetPalette.setColor(QPalette::Window, Qt::white);
    setPalette(widgetPalette);

    ui->titleBar->installEventFilter(this);
    ui->labelWindowTitle->installEventFilter(this);

    m_pCreateModeButtonGroup->setExclusive(true);
    m_pCreateModeButtonGroup->addButton(ui->buttonCreateMode);
    m_pCreateModeButtonGroup->addButton(ui->buttonImportMode);

    m_pExperimentTypeButtonGroup->setExclusive(true);
    m_pExperimentTypeButtonGroup->addButton(ui->buttonConfluenceAnalysis);
    m_pExperimentTypeButtonGroup->addButton(ui->buttonScratchAnalysis);
    m_pExperimentTypeButtonGroup->addButton(ui->buttonTransfectionAnalysis);
    m_pExperimentTypeButtonGroup->addButton(ui->buttonCellActivityAnalysis);

    m_pScanModeButtonGroup->setExclusive(true);
    m_pScanModeButtonGroup->addButton(ui->buttonLoopScanMode);
    m_pScanModeButtonGroup->addButton(ui->buttonSingleScanMode);

    ui->buttonCreateMode->setChecked(true);
    ui->buttonSingleScanMode->setChecked(true);

    ui->checkBrightField->setChecked(true);
    ui->checkBlueFluorescence->setChecked(false);
    ui->checkGreenFluorescence->setChecked(false);
    ui->checkRedFluorescence->setChecked(false);

    ui->checkPage5FocusBrightField->setChecked(true);
    ui->checkPage5FocusBlueFluorescence->setChecked(false);
    ui->checkPage5FocusGreenFluorescence->setChecked(false);
    ui->checkPage5FocusRedFluorescence->setChecked(false);

    ui->lineEditIntervalHours->setText(QStringLiteral("0"));
    ui->lineEditIntervalMinutes->setText(QStringLiteral("10"));
    ui->lineEditTotalHours->setText(QStringLiteral("0"));
    ui->lineEditTotalMinutes->setText(QStringLiteral("30"));
    ui->lineEditFieldOverlap->setText(QStringLiteral("0.05"));
    ui->lineEditFocusInterval->setText(QStringLiteral("1"));
    resetPlateFieldSelectionSummary();

    updateDelaySettingState();
    resetToFirstPage();
}

void CreateExperimentSubPage::initConnections()
{
    connect(ui->buttonClose, &QPushButton::clicked, this, &QWidget::hide);
    connect(ui->buttonPreviousPage, &QPushButton::clicked,
        this, &CreateExperimentSubPage::goPreviousPage);
    connect(ui->buttonNextPage, &QPushButton::clicked,
        this, &CreateExperimentSubPage::goNextPage);
    connect(ui->buttonChoosePlateFields, &QPushButton::clicked,
        this, &CreateExperimentSubPage::choosePlateFieldsRequested);
    connect(ui->buttonLoopScanMode, &QPushButton::toggled,
        this, &CreateExperimentSubPage::updateDelaySettingState);
    connect(ui->lineEditIntervalHours, &QLineEdit::textChanged,
        this, &CreateExperimentSubPage::updateAutoCycleCount);
    connect(ui->lineEditIntervalMinutes, &QLineEdit::textChanged,
        this, &CreateExperimentSubPage::updateAutoCycleCount);
    connect(ui->lineEditTotalHours, &QLineEdit::textChanged,
        this, &CreateExperimentSubPage::updateAutoCycleCount);
    connect(ui->lineEditTotalMinutes, &QLineEdit::textChanged,
        this, &CreateExperimentSubPage::updateAutoCycleCount);
}

void CreateExperimentSubPage::centerInContainer(QWidget *container)
{
    if (container == nullptr)
    {
        show();
        raise();
        return;
    }

    const QSize targetSize = sizeHint().expandedTo(minimumSize()).boundedTo(
        QSize(qMax(620, container->width() - 40), qMax(500, container->height() - 40)));
    resize(targetSize);

    const int x = (container->width() - width()) / 2;
    const int y = (container->height() - height()) / 2;
    move(qMax(0, x), qMax(0, y));

    show();
    raise();
}

void CreateExperimentSubPage::updateDelaySettingState()
{
    const bool enabled = isLoopScanMode();
    ui->lineEditIntervalHours->setEnabled(enabled);
    ui->lineEditIntervalMinutes->setEnabled(enabled);
    ui->lineEditTotalHours->setEnabled(enabled);
    ui->lineEditTotalMinutes->setEnabled(enabled);

    updateAutoCycleCount();
}

void CreateExperimentSubPage::updateAutoCycleCount()
{
    ui->labelAutoCycleCountValue->setText(QStringLiteral("%1次").arg(calculatedCycleCount()));
}

void CreateExperimentSubPage::resetPlateFieldSelectionSummary()
{
    ui->lineEditPage4PlateType->clear();
    ui->lineEditPage4SelectedWells->clear();
    ui->lineEditPage4SelectedGroups->clear();
    ui->lineEditPage4SelectedFields->clear();
    ui->lineSummaryPlateType->clear();
    ui->lineSummarySelectedWells->clear();
    ui->lineSummarySelectedGroups->clear();
    ui->lineSummarySelectedFields->clear();
    ui->page4StateStackedWidget->setCurrentWidget(ui->page4StateEmpty);
}

bool CreateExperimentSubPage::isLoopScanMode() const
{
    return ui->buttonLoopScanMode->isChecked();
}

int CreateExperimentSubPage::intervalDurationMinutes() const
{
    return lineEditToNonNegativeInt(ui->lineEditIntervalHours) * 60
        + lineEditToNonNegativeInt(ui->lineEditIntervalMinutes);
}

int CreateExperimentSubPage::totalDurationMinutes() const
{
    return lineEditToNonNegativeInt(ui->lineEditTotalHours) * 60
        + lineEditToNonNegativeInt(ui->lineEditTotalMinutes);
}

int CreateExperimentSubPage::calculatedCycleCount() const
{
    if (!isLoopScanMode())
    {
        return 1;
    }

    const int intervalMinutes = intervalDurationMinutes();
    const int totalMinutes = totalDurationMinutes();
    if (intervalMinutes <= 0 || totalMinutes <= 0)
    {
        return 0;
    }

    return totalMinutes / intervalMinutes;
}

void CreateExperimentSubPage::updatePageIndicator()
{
    const QList<QLabel *> labels = {
        ui->labelDot1,
        ui->labelDot2,
        ui->labelDot3,
        ui->labelDot4,
        ui->labelDot5,
        ui->labelDot6
    };

    const int currentIndex = ui->pageStackedWidget->currentIndex();
    for (int index = 0; index < labels.count(); ++index)
    {
        labels.at(index)->setText(index == currentIndex ? QStringLiteral("●") : QStringLiteral("○"));
    }
}

void CreateExperimentSubPage::goPreviousPage()
{
    const int nextIndex = qMax(0, ui->pageStackedWidget->currentIndex() - 1);
    ui->pageStackedWidget->setCurrentIndex(nextIndex);
    updatePageIndicator();
}

void CreateExperimentSubPage::goNextPage()
{
    if (!validateCurrentPage())
    {
        return;
    }

    const int currentIndex = ui->pageStackedWidget->currentIndex();
    if (currentIndex == ui->pageStackedWidget->count() - 1)
    {
        emit experimentPageAccepted(AcceptedPage::Summary);
        emit experimentFinished();
        hide();
        return;
    }

    emit experimentPageAccepted(static_cast<AcceptedPage>(currentIndex));
    ui->pageStackedWidget->setCurrentIndex(qMin(ui->pageStackedWidget->count() - 1, currentIndex + 1));
    updatePageIndicator();
}

bool CreateExperimentSubPage::validateCurrentPage()
{
    if (ui->pageStackedWidget->currentWidget() == ui->pageStep1)
    {
        return validateFirstPage();
    }
    if (ui->pageStackedWidget->currentWidget() == ui->pageStep2)
    {
        return validateExperimentTypePage();
    }
    if (ui->pageStackedWidget->currentWidget() == ui->pageStep4)
    {
        return validatePlateFieldPage();
    }
    if (ui->pageStackedWidget->currentWidget() == ui->pageStep5)
    {
        return validateScanParameterPage();
    }

    return true;
}

bool CreateExperimentSubPage::validateFirstPage()
{
    if (ui->lineEditExperimentName->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先输入实验方法名称。"));
        ui->lineEditExperimentName->setFocus();
        return false;
    }

    return true;
}

bool CreateExperimentSubPage::validateExperimentTypePage()
{
    if (experimentType() == QStringLiteral("-"))
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先选择实验类型。"));
        return false;
    }

    if (scanChannelText() == QStringLiteral("-"))
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请至少选择一个扫描通道。"));
        return false;
    }

    return true;
}

bool CreateExperimentSubPage::validatePlateFieldPage()
{
    const bool hasPlateFields = ui->page4StateStackedWidget->currentWidget() == ui->page4StateSummary
        && !ui->lineEditPage4SelectedWells->text().trimmed().isEmpty()
        && !ui->lineEditPage4SelectedFields->text().trimmed().isEmpty();
    if (!hasPlateFields)
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先选择孔板和视野。"));
        return false;
    }

    return true;
}

bool CreateExperimentSubPage::validateScanParameterPage()
{
    if (focusChannelText() == QStringLiteral("-"))
    {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请至少选择一个对焦通道。"));
        return false;
    }

    return true;
}

QString CreateExperimentSubPage::checkedButtonText(const QButtonGroup *buttonGroup) const
{
    const QAbstractButton *pButton = buttonGroup->checkedButton();
    if (pButton == nullptr)
    {
        return QString();
    }

    const QString experimentType = pButton->property("experimentType").toString();
    if (!experimentType.isEmpty())
    {
        return experimentType;
    }

    const QString scanMode = pButton->property("scanMode").toString();
    if (!scanMode.isEmpty())
    {
        return scanMode;
    }

    return pButton->text().split(QLatin1Char('\n')).first().trimmed();
}

QString CreateExperimentSubPage::checkedChannelText() const
{
    QStringList channels;
    if (ui->checkBrightField->isChecked())
    {
        channels << ui->checkBrightField->text();
    }
    if (ui->checkBlueFluorescence->isChecked())
    {
        channels << ui->checkBlueFluorescence->text();
    }
    if (ui->checkGreenFluorescence->isChecked())
    {
        channels << ui->checkGreenFluorescence->text();
    }
    if (ui->checkRedFluorescence->isChecked())
    {
        channels << ui->checkRedFluorescence->text();
    }

    return channels.join(QStringLiteral(" "));
}

QString CreateExperimentSubPage::checkedFocusChannelText() const
{
    QStringList channels;
    if (ui->checkPage5FocusBrightField->isChecked())
    {
        channels << ui->checkPage5FocusBrightField->text();
    }
    if (ui->checkPage5FocusBlueFluorescence->isChecked())
    {
        channels << ui->checkPage5FocusBlueFluorescence->text();
    }
    if (ui->checkPage5FocusGreenFluorescence->isChecked())
    {
        channels << ui->checkPage5FocusGreenFluorescence->text();
    }
    if (ui->checkPage5FocusRedFluorescence->isChecked())
    {
        channels << ui->checkPage5FocusRedFluorescence->text();
    }

    return channels.join(QStringLiteral(" "));
}
