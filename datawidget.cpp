#include "datawidget.h"
#include "ui_datawidget.h"

#include <QDateTime>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>

DataWidget::DataWidget(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::DataWidget)
{
    m_pUi->setupUi(this);

    initExperimentTable();
    initConnections();
}

DataWidget::~DataWidget()
{
    delete m_pUi;
}

void DataWidget::initExperimentTable()
{
    m_pUi->tableExperimentList->setColumnCount(6);
    m_pUi->tableExperimentList->setHorizontalHeaderLabels(QStringList()
        << QStringLiteral("序号")
        << QStringLiteral("编号")
        << QStringLiteral("实验名称")
        << QStringLiteral("实验类型")
        << QStringLiteral("扫描类型")
        << QStringLiteral("实验日期"));
    m_pUi->tableExperimentList->horizontalHeader()->setStretchLastSection(true);
    m_pUi->tableExperimentList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pUi->tableExperimentList->verticalHeader()->setVisible(false);
    m_pUi->tableExperimentList->setRowCount(0);

    m_pUi->buttonPageOne->setCheckable(true);
    m_pUi->buttonPageOne->setChecked(true);
    m_pUi->buttonPreviousPage->setVisible(false);
    m_pUi->buttonPageTwo->setVisible(false);
    m_pUi->buttonPageThree->setVisible(false);
    m_pUi->buttonNextPage->setVisible(false);

    const QDateTime currentDateTime = QDateTime::currentDateTime();
    m_pUi->dateTimeEnd->setDateTime(currentDateTime);
    m_pUi->dateTimeStart->setDateTime(currentDateTime.addMonths(-6));
}

void DataWidget::initConnections()
{
    connect(m_pUi->buttonExperimentDetail, &QPushButton::clicked, this, [this]() {
        m_pUi->dataPageStackedWidget->setCurrentWidget(m_pUi->experimentDetailPage);
        m_pUi->buttonGlobalList->setChecked(false);
        m_pUi->buttonExperimentDetail->setChecked(true);
    });

    connect(m_pUi->buttonGlobalList, &QPushButton::clicked, this, [this]() {
        m_pUi->dataPageStackedWidget->setCurrentWidget(m_pUi->globalListPage);
        m_pUi->buttonGlobalList->setChecked(true);
        m_pUi->buttonExperimentDetail->setChecked(false);
    });

    connect(m_pUi->buttonDetailBack, &QPushButton::clicked, this, [this]() {
        m_pUi->dataPageStackedWidget->setCurrentWidget(m_pUi->globalListPage);
        m_pUi->buttonGlobalList->setChecked(true);
        m_pUi->buttonExperimentDetail->setChecked(false);
    });
}
