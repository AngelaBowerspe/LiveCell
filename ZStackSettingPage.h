#ifndef ZSTACKSETTINGPAGE_H
#define ZSTACKSETTINGPAGE_H

#include "models/ZStackSettings.h"

#include <QLineEdit>
#include <QWidget>

namespace Ui {
class ZStackSettingPage;
}

class ZStackSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit ZStackSettingPage(QWidget *parent = nullptr);
    ~ZStackSettingPage();

    ZStackSettings zStackSettings() const;

signals:
    void zStackRequested(const ZStackSettings &settings);
    void viewResultRequested();
    void backPreviewRequested();
    void saveDataRequested();
    void zStackSettingsChanged();

private:
    void initValidators();
    void initConnections();
    int lineEditIntValue(const QLineEdit *pLineEdit) const;
    double lineEditDoubleValue(const QLineEdit *pLineEdit) const;

private:
    Ui::ZStackSettingPage *m_pUi;
};

#endif // ZSTACKSETTINGPAGE_H
