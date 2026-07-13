#ifndef SYSTEMSETTINGSSUBPAGE_H
#define SYSTEMSETTINGSSUBPAGE_H

#include "models/SystemSettings.h"

#include <QEvent>
#include <QPoint>
#include <QWidget>

namespace Ui {
class SystemSettingsSubPage;
}

class SystemSettingsSubPage : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSettingsSubPage(QWidget *parent = nullptr);
    ~SystemSettingsSubPage();

    void showCenteredIn(QWidget *container);
    void setSettings(const SystemSettings &settings);
    SystemSettings settings() const;

signals:
    void settingsAccepted(const SystemSettings &settings);
    void settingsCancelled();
    void positionCalibrationRequested();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initControls();
    void initConnections();
    void centerInContainer(QWidget *container);
    void chooseStoragePath();
    void chooseScaleColor();
    void acceptSettings();
    void cancelSettings();
    void updateScaleColorButton();

    Ui::SystemSettingsSubPage *ui;
    bool m_bDragging;
    QPoint m_dragOffset;
    QColor m_scaleColor;
};

#endif // SYSTEMSETTINGSSUBPAGE_H
