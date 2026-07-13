#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QButtonGroup>
#include <QMainWindow>

#include "models/SystemSettings.h"

class DataWidget;
class PreviewPage;
class ScanPage;
class SystemSettingsSubPage;

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
    void initSettings();
    void showSettingsPage();
    void applySystemSettings(const SystemSettings &settings);

private:
    Ui::MainWindow *m_pUi;
    QButtonGroup *m_pNavigationGroup;
    PreviewPage *m_pPreviewPage;
    ScanPage *m_pScanPage;
    DataWidget *m_pDataWidget;
    SystemSettingsSubPage *m_pSystemSettingsSubPage;
    SystemSettings m_systemSettings;
};
#endif // MAINWINDOW_H
