#ifndef BASICSETTINGPAGE_H
#define BASICSETTINGPAGE_H

#include <QWidget>

namespace Ui {
class BasicSettingPage;
}

class BasicSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit BasicSettingPage(QWidget *parent = nullptr);
    ~BasicSettingPage();

signals:
    void recordVideoToggled(bool checked);

private:
    Ui::BasicSettingPage *m_pUi;
};

#endif // BASICSETTINGPAGE_H
