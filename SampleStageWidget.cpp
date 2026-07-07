#include "SampleStageWidget.h"

#include "FieldViewWidget.h"

#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVariant>

SampleStageWidget::SampleStageWidget(QWidget *parent)
    : QWidget(parent)
    , m_pTitleLabel(nullptr)
    , m_pPlateLabel(nullptr)
    , m_pWellTitleLabel(nullptr)
    , m_pFieldTitleLabel(nullptr)
    , m_pPlateComboBox(nullptr)
    , m_pWellPlateWidget(nullptr)
    , m_pFieldViewWidget(nullptr)
{
    setObjectName("sampleStageWidget");
    setMinimumWidth(390);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    initUi();
    initConnections();
    setPlateFormat(WellPlateWidget::PlateFormat::Plate24);
}

WellPlateWidget *SampleStageWidget::wellPlateWidget() const
{
    return m_pWellPlateWidget;
}

FieldViewWidget *SampleStageWidget::fieldViewWidget() const
{
    return m_pFieldViewWidget;
}

void SampleStageWidget::setPlateFormat(WellPlateWidget::PlateFormat format)
{
    m_pWellPlateWidget->setPlateFormat(format);
    m_pFieldViewWidget->setPlateFormat(format);
    updatePlateComboText();
}

WellPlateWidget::PlateFormat SampleStageWidget::plateFormat() const
{
    return m_pWellPlateWidget->plateFormat();
}

void SampleStageWidget::setWellState(const QString &well, WellPlateWidget::WellState state)
{
    m_pWellPlateWidget->setWellState(well, state);
}

void SampleStageWidget::setWellStates(const QStringList &wells, WellPlateWidget::WellState state)
{
    m_pWellPlateWidget->setWellStates(wells, state);
}

void SampleStageWidget::clearWellState(WellPlateWidget::WellState state)
{
    m_pWellPlateWidget->clearState(state);
}

void SampleStageWidget::clearAllWellStates()
{
    m_pWellPlateWidget->clearAll();
}

void SampleStageWidget::setFieldState(int row, int column, FieldViewWidget::FieldState state)
{
    m_pFieldViewWidget->setFieldState(row, column, state);
}

void SampleStageWidget::clearFieldState(FieldViewWidget::FieldState state)
{
    m_pFieldViewWidget->clearState(state);
}

void SampleStageWidget::clearAllFieldStates()
{
    m_pFieldViewWidget->clearAll();
}

void SampleStageWidget::initUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setObjectName("sampleStageMainLayout");

    QWidget *headerWidget = new QWidget(this);
    headerWidget->setObjectName("sampleStageHeaderWidget");
    headerWidget->setMinimumHeight(42);
    headerWidget->setMaximumHeight(42);

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(14, 0, 10, 0);
    headerLayout->setSpacing(8);
    headerLayout->setObjectName("sampleStageHeaderLayout");

    m_pTitleLabel = new QLabel(QStringLiteral("样品台"), headerWidget);
    m_pTitleLabel->setObjectName("labelSampleStageTitle");

    QToolButton *settingsButton = new QToolButton(headerWidget);
    settingsButton->setObjectName("buttonSampleStageSettings");
    settingsButton->setText(QStringLiteral("⚙"));
    settingsButton->setMinimumSize(32, 32);
    settingsButton->setMaximumSize(32, 32);

    headerLayout->addWidget(m_pTitleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(settingsButton);
    mainLayout->addWidget(headerWidget);

    QWidget *plateSelectWidget = new QWidget(this);
    plateSelectWidget->setObjectName("plateSelectWidget");
    plateSelectWidget->setMinimumHeight(58);
    plateSelectWidget->setMaximumHeight(58);

    QHBoxLayout *plateSelectLayout = new QHBoxLayout(plateSelectWidget);
    plateSelectLayout->setContentsMargins(14, 8, 14, 8);
    plateSelectLayout->setSpacing(12);
    plateSelectLayout->setObjectName("plateSelectLayout");

    m_pPlateLabel = new QLabel(QStringLiteral("样品板"), plateSelectWidget);
    m_pPlateLabel->setObjectName("labelPlateFormat");

    m_pPlateComboBox = new QComboBox(plateSelectWidget);
    m_pPlateComboBox->setObjectName("comboPlateFormat");
    m_pPlateComboBox->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate6),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate6)));
    m_pPlateComboBox->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate12),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate12)));
    m_pPlateComboBox->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate24),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate24)));
    m_pPlateComboBox->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate48),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate48)));
    m_pPlateComboBox->addItem(plateFormatText(WellPlateWidget::PlateFormat::Plate96),
        QVariant::fromValue(static_cast<int>(WellPlateWidget::PlateFormat::Plate96)));
    m_pPlateComboBox->setMinimumWidth(180);

    plateSelectLayout->addWidget(m_pPlateLabel);
    plateSelectLayout->addStretch();
    plateSelectLayout->addWidget(m_pPlateComboBox);
    mainLayout->addWidget(plateSelectWidget);

    QWidget *wellPanelWidget = new QWidget(this);
    wellPanelWidget->setObjectName("wellPanelWidget");
    QVBoxLayout *wellPanelLayout = new QVBoxLayout(wellPanelWidget);
    wellPanelLayout->setContentsMargins(14, 10, 14, 12);
    wellPanelLayout->setSpacing(8);
    wellPanelLayout->setObjectName("wellPanelLayout");

    m_pWellTitleLabel = new QLabel(QStringLiteral("孔板"), wellPanelWidget);
    m_pWellTitleLabel->setObjectName("labelWellPlateTitle");

    m_pWellPlateWidget = new WellPlateWidget(wellPanelWidget);
    m_pWellPlateWidget->setObjectName("wellPlateWidget");

    QFrame *separator = new QFrame(wellPanelWidget);
    separator->setObjectName("wellPanelSeparator");
    separator->setFrameShape(QFrame::HLine);

    wellPanelLayout->addWidget(m_pWellTitleLabel);
    wellPanelLayout->addWidget(m_pWellPlateWidget);
    wellPanelLayout->addWidget(separator);
    mainLayout->addWidget(wellPanelWidget);

    QWidget *fieldPanelWidget = new QWidget(this);
    fieldPanelWidget->setObjectName("fieldPanelWidget");
    QVBoxLayout *fieldPanelLayout = new QVBoxLayout(fieldPanelWidget);
    fieldPanelLayout->setContentsMargins(28, 24, 28, 12);
    fieldPanelLayout->setSpacing(12);
    fieldPanelLayout->setObjectName("fieldPanelLayout");

    m_pFieldTitleLabel = new QLabel(QStringLiteral("视野"), fieldPanelWidget);
    m_pFieldTitleLabel->setObjectName("labelFieldViewTitle");

    m_pFieldViewWidget = new FieldViewWidget(fieldPanelWidget);
    m_pFieldViewWidget->setObjectName("fieldViewWidget");

    fieldPanelLayout->addWidget(m_pFieldTitleLabel);
    fieldPanelLayout->addWidget(m_pFieldViewWidget);
    mainLayout->addWidget(fieldPanelWidget);
    mainLayout->addStretch();
}

void SampleStageWidget::initConnections()
{
    connect(m_pPlateComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        const WellPlateWidget::PlateFormat format = static_cast<WellPlateWidget::PlateFormat>(
            m_pPlateComboBox->itemData(index).toInt());
        setPlateFormat(format);
    });
}

void SampleStageWidget::updatePlateComboText()
{
    const int target = static_cast<int>(plateFormat());
    for (int index = 0; index < m_pPlateComboBox->count(); ++index)
    {
        if (m_pPlateComboBox->itemData(index).toInt() == target)
        {
            if (m_pPlateComboBox->currentIndex() != index)
            {
                m_pPlateComboBox->setCurrentIndex(index);
            }
            return;
        }
    }
}

QString SampleStageWidget::plateFormatText(WellPlateWidget::PlateFormat format)
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

    return QStringLiteral("24孔板");
}
