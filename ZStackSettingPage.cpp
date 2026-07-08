#include "ZStackSettingPage.h"

#include "ui_ZStackSettingPage.h"

#include <QDoubleValidator>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>

ZStackSettingPage::ZStackSettingPage(QWidget *parent)
    : QWidget(parent)
    , m_pUi(new Ui::ZStackSettingPage)
{
    m_pUi->setupUi(this);

    initValidators();
    initConnections();
}

ZStackSettingPage::~ZStackSettingPage()
{
    delete m_pUi;
}

ZStackSettings ZStackSettingPage::zStackSettings() const
{
    ZStackSettings settings;
    settings.stepDistance = lineEditDoubleValue(m_pUi->lineEditStepDistance);
    settings.layerCount = lineEditIntValue(m_pUi->lineEditScanLayerCount);
    return settings;
}

void ZStackSettingPage::initValidators()
{
    m_pUi->lineEditStepDistance->setValidator(new QDoubleValidator(0.1, 9999.0, 2, this));
    m_pUi->lineEditScanLayerCount->setValidator(new QIntValidator(1, 1000, this));
}

void ZStackSettingPage::initConnections()
{
    connect(m_pUi->lineEditStepDistance, &QLineEdit::editingFinished,
        this, &ZStackSettingPage::zStackSettingsChanged);
    connect(m_pUi->lineEditScanLayerCount, &QLineEdit::editingFinished,
        this, &ZStackSettingPage::zStackSettingsChanged);

    connect(m_pUi->buttonStartZStack, &QPushButton::clicked, this, [this]() {
        emit zStackRequested(zStackSettings());
    });
    connect(m_pUi->buttonViewResult, &QPushButton::clicked,
        this, &ZStackSettingPage::viewResultRequested);
    connect(m_pUi->buttonBackPreview, &QPushButton::clicked,
        this, &ZStackSettingPage::backPreviewRequested);
    connect(m_pUi->buttonSaveZStackData, &QPushButton::clicked,
        this, &ZStackSettingPage::saveDataRequested);
}

int ZStackSettingPage::lineEditIntValue(const QLineEdit *pLineEdit) const
{
    return pLineEdit->text().toInt();
}

double ZStackSettingPage::lineEditDoubleValue(const QLineEdit *pLineEdit) const
{
    return pLineEdit->text().toDouble();
}
