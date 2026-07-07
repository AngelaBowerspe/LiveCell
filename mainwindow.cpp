#include "mainwindow.h"

#include "datawidget.h"
#include "previewpage.h"
#include "scanpage.h"
#include "ui_mainwindow.h"

#include <QButtonGroup>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_pUi(new Ui::MainWindow)
    , m_pNavigationGroup(new QButtonGroup(this))
    , m_pPreviewPage(nullptr)
    , m_pScanPage(nullptr)
    , m_pDataWidget(nullptr)
{
    m_pUi->setupUi(this);

    initPages();
    initNavigation();
}

MainWindow::~MainWindow()
{
    delete m_pUi;
}

void MainWindow::initPages()
{
    m_pPreviewPage = new PreviewPage(this);
    m_pScanPage = new ScanPage(this);
    m_pDataWidget = new DataWidget(this);

    m_pUi->pageStackedWidget->addWidget(m_pPreviewPage);
    m_pUi->pageStackedWidget->addWidget(m_pScanPage);
    m_pUi->pageStackedWidget->addWidget(m_pDataWidget);
    m_pUi->pageStackedWidget->setCurrentWidget(m_pPreviewPage);
}

void MainWindow::initNavigation()
{
    m_pNavigationGroup->setExclusive(true);
    m_pNavigationGroup->addButton(m_pUi->buttonPreviewPage, 0);
    m_pNavigationGroup->addButton(m_pUi->buttonScanPage, 1);
    m_pNavigationGroup->addButton(m_pUi->buttonDataPage, 2);

    connect(m_pNavigationGroup, &QButtonGroup::idClicked,
        m_pUi->pageStackedWidget, &QStackedWidget::setCurrentIndex);

    m_pUi->buttonPreviewPage->setChecked(true);
}
