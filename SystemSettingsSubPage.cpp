#include "SystemSettingsSubPage.h"

#include "ui_SystemSettingsSubPage.h"

#include <QButtonGroup>
#include <QColorDialog>
#include <QFileDialog>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPalette>
#include <QPushButton>

namespace {

double lineEditToDouble(const QLineEdit *lineEdit, double fallback)
{
    bool ok = false;
    const double value = lineEdit->text().trimmed().toDouble(&ok);
    return ok ? value : fallback;
}

int lineEditToInt(const QLineEdit *lineEdit, int fallback)
{
    bool ok = false;
    const int value = lineEdit->text().trimmed().toInt(&ok);
    return ok ? value : fallback;
}

void setLineEditValue(QLineEdit *lineEdit, double value)
{
    lineEdit->setText(QString::number(value, 'g', 12));
}

}

SystemSettingsSubPage::SystemSettingsSubPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemSettingsSubPage)
    , m_bDragging(false)
    , m_dragOffset()
    , m_scaleColor(Qt::red)
{
    ui->setupUi(this);

    initControls();
    initConnections();
}

SystemSettingsSubPage::~SystemSettingsSubPage()
{
    delete ui;
}

void SystemSettingsSubPage::showCenteredIn(QWidget *container)
{
    if (container == nullptr)
    {
        show();
        raise();
        return;
    }

    if (parentWidget() != container)
    {
        setParent(container);
    }

    centerInContainer(container);
}

void SystemSettingsSubPage::setSettings(const SystemSettings &settings)
{
    ui->buttonBasicSettings->setChecked(true);
    ui->pageStackedWidget->setCurrentWidget(ui->pageBasicSettings);

    ui->lineEditStoragePath->setText(settings.storagePath);

    int languageIndex = -1;
    for (int index = 0; index < ui->comboLanguage->count(); ++index)
    {
        if (ui->comboLanguage->itemText(index).trimmed() == settings.language.trimmed())
        {
            languageIndex = index;
            break;
        }
    }
    ui->comboLanguage->setCurrentIndex(languageIndex >= 0 ? languageIndex : 0);

    ui->comboImageFormat->setCurrentText(settings.imageFormat);
    ui->lineEditVideoFrameRate->setText(QString::number(settings.videoFrameRate));
    setLineEditValue(ui->lineEditBrightFieldRatio, settings.brightFieldRatio);
    setLineEditValue(ui->lineEditBlueRatio, settings.blueRatio);
    setLineEditValue(ui->lineEditGreenRatio, settings.greenRatio);
    setLineEditValue(ui->lineEditRedRatio, settings.redRatio);

    int plateTypeIndex = -1;
    for (int index = 0; index < ui->comboPlateType->count(); ++index)
    {
        if (ui->comboPlateType->itemText(index).trimmed() == settings.plateType.trimmed())
        {
            plateTypeIndex = index;
            break;
        }
    }
    ui->comboPlateType->setCurrentIndex(plateTypeIndex >= 0 ? plateTypeIndex : 0);
    if (settings.plateSubType == QStringLiteral("B"))
    {
        ui->radioPlateSubTypeB->setChecked(true);
    }
    else if (settings.plateSubType == QStringLiteral("C"))
    {
        ui->radioPlateSubTypeC->setChecked(true);
    }
    else
    {
        ui->radioPlateSubTypeA->setChecked(true);
    }
    setLineEditValue(ui->lineEditWellAreaLength, settings.wellAreaLength);
    setLineEditValue(ui->lineEditWellAreaWidth, settings.wellAreaWidth);
    setLineEditValue(ui->lineEditFirstWellX, settings.firstWellX);
    setLineEditValue(ui->lineEditFirstWellY, settings.firstWellY);
    setLineEditValue(ui->lineEditWellPitchX, settings.wellPitchX);
    setLineEditValue(ui->lineEditWellPitchY, settings.wellPitchY);
    setLineEditValue(ui->lineEditPlateRotation, settings.plateRotation);
    setLineEditValue(ui->lineEditFieldOverlap, settings.fieldOverlap);

    ui->checkScaleEnabled->setChecked(settings.scaleEnabled);
    setLineEditValue(ui->lineEditScaleLength, settings.scaleLength);
    setLineEditValue(ui->lineEditScaleLineWidth, settings.scaleLineWidth);
    setLineEditValue(ui->lineEditScaleFontSize, settings.scaleFontSize);
    m_scaleColor = settings.scaleColor;
    updateScaleColorButton();

    ui->lineEditCellSegmentationLevel->setText(QString::number(settings.cellSegmentationLevel));
    ui->lineEditCellEdgeWidth->setText(QString::number(settings.cellEdgeWidth));
    ui->lineEditCellDiameterLength->setText(QString::number(settings.cellDiameterLength));
    ui->lineEditCellAreaThreshold->setText(QString::number(settings.cellAreaThreshold));
    ui->lineEditCellBrightnessThreshold->setText(QString::number(settings.cellBrightnessThreshold));
}

SystemSettings SystemSettingsSubPage::settings() const
{
    SystemSettings result;
    result.storagePath = ui->lineEditStoragePath->text().trimmed();
    result.language = ui->comboLanguage->currentText().trimmed();
    result.imageFormat = ui->comboImageFormat->currentText();
    result.videoFrameRate = lineEditToInt(ui->lineEditVideoFrameRate, 5);
    result.brightFieldRatio = lineEditToDouble(ui->lineEditBrightFieldRatio, 1.0);
    result.blueRatio = lineEditToDouble(ui->lineEditBlueRatio, 0.25);
    result.greenRatio = lineEditToDouble(ui->lineEditGreenRatio, 0.25);
    result.redRatio = lineEditToDouble(ui->lineEditRedRatio, 0.25);

    result.plateType = ui->comboPlateType->currentText().trimmed();
    result.plateSubType = ui->radioPlateSubTypeB->isChecked()
        ? QStringLiteral("B")
        : ui->radioPlateSubTypeC->isChecked() ? QStringLiteral("C") : QStringLiteral("A");
    result.wellAreaLength = lineEditToDouble(ui->lineEditWellAreaLength, 10000.0);
    result.wellAreaWidth = lineEditToDouble(ui->lineEditWellAreaWidth, 10000.0);
    result.firstWellX = lineEditToDouble(ui->lineEditFirstWellX, 0.0);
    result.firstWellY = lineEditToDouble(ui->lineEditFirstWellY, 0.0);
    result.wellPitchX = lineEditToDouble(ui->lineEditWellPitchX, 10000.0);
    result.wellPitchY = lineEditToDouble(ui->lineEditWellPitchY, 10000.0);
    result.plateRotation = lineEditToDouble(ui->lineEditPlateRotation, 0.0);
    result.fieldOverlap = lineEditToDouble(ui->lineEditFieldOverlap, 10.0);

    result.scaleEnabled = ui->checkScaleEnabled->isChecked();
    result.scaleLength = lineEditToDouble(ui->lineEditScaleLength, 100.0);
    result.scaleLineWidth = lineEditToDouble(ui->lineEditScaleLineWidth, 2.0);
    result.scaleFontSize = lineEditToDouble(ui->lineEditScaleFontSize, 12.0);
    result.scaleColor = m_scaleColor;

    result.cellSegmentationLevel = lineEditToInt(ui->lineEditCellSegmentationLevel, 1);
    result.cellEdgeWidth = lineEditToInt(ui->lineEditCellEdgeWidth, 0);
    result.cellDiameterLength = lineEditToInt(ui->lineEditCellDiameterLength, 50);
    result.cellAreaThreshold = lineEditToInt(ui->lineEditCellAreaThreshold, 0);
    result.cellBrightnessThreshold = lineEditToInt(ui->lineEditCellBrightnessThreshold, 0);
    return result;
}

bool SystemSettingsSubPage::eventFilter(QObject *watched, QEvent *event)
{
    if ((watched == ui->titleBar || watched == ui->labelWindowTitle) && parentWidget() != nullptr)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                m_bDragging = true;
                m_dragOffset = mapFromGlobal(mouseEvent->globalPosition().toPoint());
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (m_bDragging && (mouseEvent->buttons() & Qt::LeftButton))
            {
                const QPoint globalTopLeft = mouseEvent->globalPosition().toPoint() - m_dragOffset;
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
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                m_bDragging = false;
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void SystemSettingsSubPage::initControls()
{
    setObjectName(QStringLiteral("systemSettingsSubPage"));
    setAutoFillBackground(true);

    QPalette widgetPalette = palette();
    widgetPalette.setColor(QPalette::Window, Qt::white);
    setPalette(widgetPalette);

    ui->titleBar->installEventFilter(this);
    ui->labelWindowTitle->installEventFilter(this);

    ui->buttonBasicSettings->setChecked(true);
    ui->comboLanguage->setCurrentIndex(0);
    ui->comboImageFormat->setCurrentText(QStringLiteral("JPG"));
    ui->radioPlateSubTypeA->setChecked(true);
    ui->checkScaleEnabled->setChecked(true);
    updateScaleColorButton();
}

void SystemSettingsSubPage::initConnections()
{
    QButtonGroup *pageButtonGroup = new QButtonGroup(this);
    pageButtonGroup->setExclusive(true);
    pageButtonGroup->addButton(ui->buttonBasicSettings, 0);
    pageButtonGroup->addButton(ui->buttonPlateSettings, 1);
    pageButtonGroup->addButton(ui->buttonScaleSettings, 2);
    pageButtonGroup->addButton(ui->buttonMonitoringSettings, 3);

    connect(pageButtonGroup, &QButtonGroup::idClicked, this, [this](int pageIndex) {
        ui->pageStackedWidget->setCurrentIndex(pageIndex);
    });
    connect(ui->buttonClose, &QPushButton::clicked, this, &SystemSettingsSubPage::cancelSettings);
    connect(ui->buttonCancel, &QPushButton::clicked, this, &SystemSettingsSubPage::cancelSettings);
    connect(ui->buttonConfirm, &QPushButton::clicked, this, &SystemSettingsSubPage::acceptSettings);
    connect(ui->buttonBrowseStorage, &QPushButton::clicked,
        this, &SystemSettingsSubPage::chooseStoragePath);
    connect(ui->buttonPositionCalibration, &QPushButton::clicked,
        this, &SystemSettingsSubPage::positionCalibrationRequested);
    connect(ui->buttonScaleColor, &QPushButton::clicked,
        this, &SystemSettingsSubPage::chooseScaleColor);
}

void SystemSettingsSubPage::centerInContainer(QWidget *container)
{
    const QSize targetSize = sizeHint().expandedTo(minimumSize()).boundedTo(
        QSize(qMax(700, container->width() - 40), qMax(520, container->height() - 40)));
    resize(targetSize);

    const int x = (container->width() - width()) / 2;
    const int y = (container->height() - height()) / 2;
    move(qMax(0, x), qMax(0, y));

    show();
    raise();
}

void SystemSettingsSubPage::chooseStoragePath()
{
    const QString path = QFileDialog::getExistingDirectory(this,
        QStringLiteral("选择文件存储目录"),
        ui->lineEditStoragePath->text());
    if (!path.isEmpty())
    {
        ui->lineEditStoragePath->setText(path);
    }
}

void SystemSettingsSubPage::chooseScaleColor()
{
    const QColor color = QColorDialog::getColor(m_scaleColor, this,
        QStringLiteral("选择比例尺颜色"));
    if (color.isValid())
    {
        m_scaleColor = color;
        updateScaleColorButton();
    }
}

void SystemSettingsSubPage::acceptSettings()
{
    emit settingsAccepted(settings());
    hide();
}

void SystemSettingsSubPage::cancelSettings()
{
    emit settingsCancelled();
    hide();
}

void SystemSettingsSubPage::updateScaleColorButton()
{
    QPalette buttonPalette = ui->buttonScaleColor->palette();
    buttonPalette.setColor(QPalette::Button, m_scaleColor);
    ui->buttonScaleColor->setAutoFillBackground(true);
    ui->buttonScaleColor->setPalette(buttonPalette);
    ui->buttonScaleColor->update();
}
