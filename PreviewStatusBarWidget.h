#ifndef PREVIEWSTATUSBARWIDGET_H
#define PREVIEWSTATUSBARWIDGET_H

#include <QWidget>

namespace Ui {
class PreviewStatusBarWidget;
}

class PreviewStatusBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewStatusBarWidget(QWidget *parent = nullptr);
    ~PreviewStatusBarWidget();

private:
    Ui::PreviewStatusBarWidget *m_pUi;
};

#endif // PREVIEWSTATUSBARWIDGET_H
