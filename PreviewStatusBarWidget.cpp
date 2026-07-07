#include "PreviewStatusBarWidget.h"

#include "ui_PreviewStatusBarWidget.h"

PreviewStatusBarWidget::PreviewStatusBarWidget(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::PreviewStatusBarWidget)
{
    m_pUi->setupUi(this);
}

PreviewStatusBarWidget::~PreviewStatusBarWidget()
{
    delete m_pUi;
}
