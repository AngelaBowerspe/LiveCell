#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>

namespace Ui {
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataWidget(QWidget *parent = nullptr);
    ~DataWidget();

private:
    void initExperimentTable();
    void initConnections();

private:
    Ui::DataWidget *m_pUi;
};

#endif // DATAWIDGET_H
