#ifndef ZSTACKSETTINGPAGE_H
#define ZSTACKSETTINGPAGE_H

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

private:
    Ui::ZStackSettingPage *m_pUi;
};

#endif // ZSTACKSETTINGPAGE_H
