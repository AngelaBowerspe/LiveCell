#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QButtonGroup;
class DataWidget;
class PreviewPage;
class ScanPage;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initPages();
    void initNavigation();

private:
    Ui::MainWindow *m_pUi;
    QButtonGroup *m_pNavigationGroup;
    PreviewPage *m_pPreviewPage;
    ScanPage *m_pScanPage;
    DataWidget *m_pDataWidget;
};
#endif // MAINWINDOW_H
