#include "ImageCorrectionPage.h"

#include "ui_ImageCorrectionPage.h"

ImageCorrectionPage::ImageCorrectionPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::ImageCorrectionPage)
{
    m_pUi->setupUi(this);
}

ImageCorrectionPage::~ImageCorrectionPage()
{
    delete m_pUi;
}
