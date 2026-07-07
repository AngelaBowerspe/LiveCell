#ifndef SCANPAGE_H
#define SCANPAGE_H

#include <QWidget>

namespace Ui {
class ScanPage;
}

class ScanPage : public QWidget
{
    Q_OBJECT

public:
    explicit ScanPage(QWidget *parent = nullptr);
    ~ScanPage();

private:
    Ui::ScanPage *ui;
};

#endif // SCANPAGE_H
